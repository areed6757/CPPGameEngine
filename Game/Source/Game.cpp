#include <Game.h>
#include <Core/Logger.h>

/*
* Game handles creation and destruction of the base client and owns the single Logger referenced by other classes.
* The Base class is constructed with {} to pass the BaseDesc (descriptor), and requires a non-null reference to the
* Logger. The Logger instance is then detatched from the unique ptr via .release() and then dereferenced with *.
* 
* Further instantiation of classes is handled in order of first created = last closed.
*/
Engine::Game::Game(const GameDesc& desc) :
	Base({ *std::make_unique<Logger>(desc.logLevel).release() }),
	m_loggerPtr(&m_logger)
{

	// Graphics / Window / Input
	GLFWDesc glfwDesc{ { m_logger } };
	m_glfwContext = std::make_unique<GLFWContext>(glfwDesc);
	WindowDesc windowDesc{ {m_logger}, desc.windowWidth, desc.windowHeight, desc.title, m_actionMap };
	m_window = std::make_unique<Window>(windowDesc);

	m_inputHandler = m_window->getInputHandler();

	stbi_set_flip_vertically_on_load(true);

	CameraDesc camDesc{ {m_logger} };
	m_camera = std::make_unique<Camera>(camDesc);

	CameraControllerDesc camContrDesc{ {m_logger}, *m_camera.get(), *m_window->getInputHandler() };
	m_cameraController = std::make_unique<CameraController>(camContrDesc);

	ShaderDesc shaderDesc{ {m_logger} };
	RendererDesc rendererDesc{ {m_logger}, *m_window.get(), shaderDesc, *m_camera.get()};
	m_renderer = std::make_unique<Renderer>(rendererDesc);

	ShaderDesc debugLineShaderDesc{ {m_logger}, "Shaders/debugline.vert", "Shaders/debugline.frag" };
	DebugLineRendererDesc debugLineRendererDesc{ {m_logger}, debugLineShaderDesc };
	m_debugLineRenderer = std::make_unique<DebugLineRenderer>(debugLineRendererDesc);

	TextureRegistryDesc textureRegDesc{ {m_logger} };
	m_textureRegistry = std::make_unique<TextureRegistry>(textureRegDesc);

	MeshRegistryDesc meshRegDesc{ {m_logger} };
	m_meshRegistry = std::make_unique<MeshRegistry>(meshRegDesc);

	// Time + Space

	GameClockDesc clockDesc = { {m_logger} };
	m_gameClock = std::make_unique<GameClock>(clockDesc);

	SchedulerDesc schedulerDesc = { {m_logger}, *m_gameClock };
	m_scheduler = std::make_unique<Scheduler>(schedulerDesc);

	QuadTreeDesc qtDesc = { {m_logger} };
	m_quadtree = std::make_unique<QuadTree>(qtDesc);

	QuadtreeDebugSystemDesc qtDebugDesc{ {m_logger}, *m_quadtree.get(), *m_camera.get(), *m_window.get(), *m_debugLineRenderer.get() };
	m_quadtreeDebugSystem = std::make_unique<QuadtreeDebugSystem>(qtDebugDesc);

	// ECS

	EntityRegisterDesc eRegDesc = { {m_logger} };
	m_entityRegister = std::make_unique<EntityRegister>(eRegDesc);

	ComponentDesc compDesc = { {m_logger} };
	ECSWrapperDesc ecsDesc = { {m_logger}, *m_entityRegister.get(), compDesc };
	m_ecsWrapper = std::make_unique<GameECSWrapper>(ecsDesc);

	// TickedSystems
	RenderSystemDesc renderSysDesc = { {m_logger}, *m_ecsWrapper.get(), *m_meshRegistry.get(), *m_textureRegistry.get(), *m_renderer.get(), *m_camera.get() };
	m_renderSystem = std::make_unique<RenderSystem>(renderSysDesc);

	CollisionSystemDesc collisionSysDesc{ {m_logger}, *m_ecsWrapper.get(), *m_quadtree.get() };
	m_collisionSystem = std::make_unique<CollisionSystem>(collisionSysDesc);

	MovementTicksDesc mvTicksDesc = { {m_logger}, *m_ecsWrapper.get(), *m_collisionSystem.get()};
	m_moveTicks = std::make_unique<MovementTicks>(mvTicksDesc);

	ThrusterSystemDesc thrSysDesc = { {m_logger}, *m_ecsWrapper.get() };
	m_thrusterSystem = std::make_unique<ThrusterSystem>(thrSysDesc);

	LifetimeSystemDesc ltsDesc = { {m_logger}, *m_ecsWrapper.get() };
	m_lifetimeSystem = std::make_unique<LifetimeSystem>(ltsDesc);

	DamageSystemDesc dsDesc = { {m_logger}, *m_ecsWrapper.get(), *m_collisionSystem.get() };
	m_damageSystem = std::make_unique<DamageSystem>(dsDesc);

	// Register TickedSystems
	m_scheduler->registerFrameSystem(m_renderSystem.get()); // Frame based update not backend ticks, smooths lag and stops buffer queueing stutter
	m_scheduler->registerFrameSystem(m_cameraController.get());
	m_scheduler->registerFrameSystem(m_quadtreeDebugSystem.get());

	m_scheduler->registerSystem(m_thrusterSystem.get());
	m_scheduler->registerSystem(m_collisionSystem.get());
	m_scheduler->registerSystem(m_moveTicks.get()); // Consumer of thruster, collision systems, register after
	m_scheduler->registerSystem(m_lifetimeSystem.get());
	m_scheduler->registerSystem(m_damageSystem.get());

	EngineLogInfo("Game initialized successfully.");

	CoreSystemsTestDesc cstDesc{ {m_logger}, *m_ecsWrapper.get() };
	m_coreSystemsTest = std::make_unique<CoreSystemsTest>(cstDesc);
	m_coreSystemsTest->spawnAll();
}

Engine::Game::~Game()
{
	EngineLogInfo("Game shutting down...");
}

void Engine::Game::run()
{
	while (!glfwWindowShouldClose(m_window->get())) {
		glfwPollEvents();

		if (m_inputHandler->wasEventActivated("pause")) {
			m_scheduler.get()->togglePause();
		}

		m_renderer->beginFrame();
		m_scheduler.get()->advance();
		m_renderer->endFrame();

		m_inputHandler->endFrame();
	}
}