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
	if (!m_window) { EngineLogErrorAndThrow("Window failed to initialize.") };

	m_inputHandler = m_window->getInputHandler();
	if (!m_inputHandler) { EngineLogErrorAndThrow("InputHandler failed to initialize.") };

	stbi_set_flip_vertically_on_load(true);

	CameraDesc camDesc{ {m_logger} };
	m_camera = std::make_unique<Camera>(camDesc);
	if (!m_camera) { EngineLogErrorAndThrow("Camera failed to initialize.") };

	CameraControllerDesc camContrDesc{ {m_logger}, *m_camera.get(), *m_window->getInputHandler() };
	m_cameraController = std::make_unique<CameraController>(camContrDesc);
	if (!m_cameraController) { EngineLogErrorAndThrow("CameraController failed to initialize.") };

	ShaderDesc shaderDesc{ {m_logger} };
	RendererDesc rendererDesc{ {m_logger}, *m_window.get(), shaderDesc, *m_camera.get()};
	m_renderer = std::make_unique<Renderer>(rendererDesc);
	if (!m_renderer) { EngineLogErrorAndThrow("Renderer failed to initialize.") };

	ShaderDesc debugLineShaderDesc{ {m_logger}, "Shaders/debugline.vert", "Shaders/debugline.frag" };
	DebugLineRendererDesc debugLineRendererDesc{ {m_logger}, debugLineShaderDesc };
	m_debugLineRenderer = std::make_unique<DebugLineRenderer>(debugLineRendererDesc);
	if (!m_debugLineRenderer) { EngineLogErrorAndThrow("DebugLineRenderer failed to initialize.") };

	TextureRegistryDesc textureRegDesc{ {m_logger} };
	m_textureRegistry = std::make_unique<TextureRegistry>(textureRegDesc);
	if (!m_textureRegistry) { EngineLogErrorAndThrow("TextureRegistry failed to initialize.") };

	MeshRegistryDesc meshRegDesc{ {m_logger} };
	m_meshRegistry = std::make_unique<MeshRegistry>(meshRegDesc);
	if (!m_meshRegistry) { EngineLogErrorAndThrow("MeshRegistry failed to initialize.") };

	// Time + Space

	GameClockDesc clockDesc = { {m_logger} };
	m_gameClock = std::make_unique<GameClock>(clockDesc);
	if (!m_gameClock) { EngineLogErrorAndThrow("GameClock failed to initialize.") };

	SchedulerDesc schedulerDesc = { {m_logger}, *m_gameClock };
	m_scheduler = std::make_unique<Scheduler>(schedulerDesc);
	if (!m_scheduler) { EngineLogErrorAndThrow("Scheduler failed to initialize.") };

	QuadTreeDesc qtDesc = { {m_logger}, Vector2double{-50.0, -50.0}, Vector2double{50.0, 50.0} };
	m_quadtree = std::make_unique<QuadTree>(qtDesc);
	if (!m_quadtree) { EngineLogErrorAndThrow("Quadtree failed to initialize") };

	QuadtreeDebugSystemDesc qtDebugDesc{ {m_logger}, *m_quadtree.get(), *m_camera.get(), *m_window.get(), *m_debugLineRenderer.get() };
	m_quadtreeDebugSystem = std::make_unique<QuadtreeDebugSystem>(qtDebugDesc);
	if (!m_quadtreeDebugSystem) { EngineLogErrorAndThrow("QuadtreeDebugSystem failed to initialize.") };

	// ECS

	EntityRegisterDesc eRegDesc = { {m_logger} };
	m_entityRegister = std::make_unique<EntityRegister>(eRegDesc);
	if (!m_entityRegister) { EngineLogErrorAndThrow("EntityRegister failed to initialize.") }

	ComponentDesc compDesc = { {m_logger} };
	ECSWrapperDesc ecsDesc = { {m_logger}, *m_entityRegister.get(), compDesc };
	m_ecsWrapper = std::make_unique<GameECSWrapper>(ecsDesc);
	if (!m_ecsWrapper) { EngineLogErrorAndThrow("ECSWrapper failed to initialize."); }

	// TickedSystems
	RenderSystemDesc renderSysDesc = { {m_logger}, *m_ecsWrapper.get(), *m_meshRegistry.get(), *m_textureRegistry.get(), *m_renderer.get(), *m_camera.get() };
	m_renderSystem = std::make_unique<RenderSystem>(renderSysDesc);
	if (!m_renderSystem) { EngineLogErrorAndThrow("Render system failed to initialize."); }

	CollisionSystemDesc collisionSysDesc{ {m_logger}, *m_ecsWrapper.get(), *m_quadtree.get() };
	m_collisionSystem = std::make_unique<CollisionSystem>(collisionSysDesc);
	if (!m_collisionSystem) { EngineLogErrorAndThrow("CollisionSystem failed to initialize.") };

	MovementTicksDesc mvTicksDesc = { {m_logger}, *m_ecsWrapper.get(), *m_collisionSystem.get()};
	m_moveTicks = std::make_unique<MovementTicks>(mvTicksDesc);
	if (!m_moveTicks) { EngineLogErrorAndThrow("MoveTicks failed to initialize.") };

	// Register TickedSystems
	m_scheduler->registerFrameSystem(m_renderSystem.get()); // Frame based update not backend ticks, smooths lag and stops buffer queueing stutter
	m_scheduler->registerFrameSystem(m_cameraController.get());
	m_scheduler->registerFrameSystem(m_quadtreeDebugSystem.get());

	m_scheduler->registerSystem(m_moveTicks.get());
	m_scheduler->registerSystem(m_collisionSystem.get());

	EngineLogInfo("Game initialized successfully.");

	// Renderable entity test with movement
	RenderGridTestDesc gridTestDesc{ {m_logger}, *m_ecsWrapper };
	m_gridTest = std::make_unique<RenderGridTest>(gridTestDesc);
	m_gridTest->spawnGrid(100, 100, 1.0);
	m_gridTest->spawnProjectiles(1, 100.0, 5.0f, 0.05f);
	m_scheduler->registerFrameSystem(m_gridTest.get());
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