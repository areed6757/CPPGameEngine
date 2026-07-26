#include <GameLayer.h>

namespace Engine {
	GameLayer::GameLayer(Application& app) :
		Base({ app.getLogger() }),
		Layer("GameLayer"),
		m_app(app)
	{
	}

	GameLayer::~GameLayer() {}

	void GameLayer::onAttach()
	{
		stbi_set_flip_vertically_on_load(true);

		CameraDesc camDesc{ {m_logger} };
		m_camera = std::make_unique<Camera>(camDesc);

		CameraControllerDesc camContrDesc{ {m_logger}, *m_camera.get(), m_app.getInputHandler() };
		m_cameraController = std::make_unique<CameraController>(camContrDesc);

		ShaderDesc shaderDesc{ {m_logger} };
		RendererDesc rendererDesc{ {m_logger}, m_app.getWindow(), shaderDesc, *m_camera.get() };
		m_renderer = std::make_unique<Renderer>(rendererDesc);

		TextureRegistryDesc textureRegDesc{ {m_logger} };
		m_textureRegistry = std::make_unique<TextureRegistry>(textureRegDesc);

		MeshRegistryDesc meshRegDesc{ {m_logger} };
		m_meshRegistry = std::make_unique<MeshRegistry>(meshRegDesc);

		QuadTreeDesc qtDesc = { {m_logger} };
		m_quadtree = std::make_unique<QuadTree>(qtDesc);

		EntityRegisterDesc eRegDesc = { {m_logger} };
		m_entityRegister = std::make_unique<EntityRegister>(eRegDesc);

		ComponentDesc compDesc = { {m_logger} };
		ECSWrapperDesc ecsDesc = { {m_logger}, *m_entityRegister.get(), compDesc };
		m_ecsWrapper = std::make_unique<GameECSWrapper>(ecsDesc);

		RenderSystemDesc renderSysDesc = { {m_logger}, *m_ecsWrapper.get(), *m_meshRegistry.get(), *m_textureRegistry.get(), *m_renderer.get(), *m_camera.get() };
		m_renderSystem = std::make_unique<RenderSystem>(renderSysDesc);

		CollisionSystemDesc collisionSysDesc{ {m_logger}, *m_ecsWrapper.get(), *m_quadtree.get() };
		m_collisionSystem = std::make_unique<CollisionSystem>(collisionSysDesc);

		ImpulseSystemDesc impulseSysDesc{ {m_logger}, *m_ecsWrapper.get(), *m_collisionSystem.get() };
		m_impulseSystem = std::make_unique<ImpulseSystem>(impulseSysDesc);

		MovementTicksDesc mvTicksDesc = { {m_logger}, *m_ecsWrapper.get(), m_app.getThreadPool() };
		m_moveTicks = std::make_unique<MovementTicks>(mvTicksDesc);

		ThrusterSystemDesc thrSysDesc = { {m_logger}, *m_ecsWrapper.get() };
		m_thrusterSystem = std::make_unique<ThrusterSystem>(thrSysDesc);

		LifetimeSystemDesc ltsDesc = { {m_logger}, *m_ecsWrapper.get() };
		m_lifetimeSystem = std::make_unique<LifetimeSystem>(ltsDesc);

		DamageSystemDesc dsDesc = { {m_logger}, *m_ecsWrapper.get(), *m_collisionSystem.get() };
		m_damageSystem = std::make_unique<DamageSystem>(dsDesc);

		ParticleSystemDesc psDesc = { {m_logger}, *m_ecsWrapper.get(), *m_camera.get(), m_app.getWindow() };
		m_particleSystem = std::make_unique<ParticleSystem>(psDesc);

		Scheduler& scheduler = m_app.getScheduler();
		scheduler.registerFrameSystem(m_renderSystem.get());
		scheduler.registerFrameSystem(m_cameraController.get());
		scheduler.registerFrameSystem(m_particleSystem.get());

		scheduler.registerSystem(m_thrusterSystem.get());
		scheduler.registerSystem(m_collisionSystem.get());
		scheduler.registerSystem(m_impulseSystem.get());
		scheduler.registerSystem(m_moveTicks.get());
		scheduler.registerSystem(m_lifetimeSystem.get());
		scheduler.registerSystem(m_damageSystem.get());

		scheduler.registerFlushCallback([this]() { m_lifetimeSystem->getCommandBuffer().flush(); });
		scheduler.registerFlushCallback([this]() { m_damageSystem->getCommandBuffer().flush(); });

		m_app.getJobController().addOrderingConstraint(m_collisionSystem.get(), m_moveTicks.get());
		m_app.getJobController().addOrderingConstraint(m_collisionSystem.get(), m_damageSystem.get());

		EngineLogInfo("GameLayer attached, game initialized successfully.");

		CoreSystemsTestDesc cstDesc{ {m_logger}, *m_ecsWrapper.get() };
		m_coreSystemsTest = std::make_unique<CoreSystemsTest>(cstDesc);
		m_coreSystemsTest->spawnAll();
	}

	void GameLayer::onDetach()
	{
		EngineLogInfo("GameLayer detached, game shutting down...");
	}

	void GameLayer::onUpdate(d64 dt)
	{
		m_renderer->beginFrame();
		m_app.getScheduler().advance();
		m_renderer->endFrame();
	}
}