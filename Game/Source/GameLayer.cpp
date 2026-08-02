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

		AABBTreeDesc aabbTreeDesc{ {m_logger}, 0.1f, 4.0f };
		m_AABBTree = std::make_unique<AABBTree>(aabbTreeDesc);

		unsigned int hwThreads = std::thread::hardware_concurrency();
		i32 collisionThreadCount = (hwThreads > 1) ? static_cast<i32>(hwThreads - 1) : 1;
		ThreadPoolDesc collisionTpDesc{ {m_logger}, collisionThreadCount };
		m_collisionThreadPool = std::make_unique<ThreadPool>(collisionTpDesc);

		EntityRegisterDesc eRegDesc = { {m_logger} };
		m_entityRegister = std::make_unique<EntityRegister>(eRegDesc);

		ComponentDesc compDesc = { {m_logger} };
		ECSWrapperDesc ecsDesc = { {m_logger}, *m_entityRegister.get(), compDesc };
		m_ecsWrapper = std::make_unique<GameECSWrapper>(ecsDesc);

		RenderSystemDesc renderSysDesc = { {m_logger}, *m_ecsWrapper.get(), *m_meshRegistry.get(), *m_textureRegistry.get(), *m_renderer.get(), *m_camera.get(), m_app.getWindow() };
		m_renderSystem = std::make_unique<RenderSystem>(renderSysDesc);

		CollisionSystemDesc collisionSysDesc{ {m_logger}, *m_ecsWrapper.get(), *m_AABBTree.get(), *m_collisionThreadPool.get() };
		m_collisionSystem = std::make_unique<CollisionSystem>(collisionSysDesc);

		ImpulseSystemDesc impulseSysDesc{ {m_logger}, *m_ecsWrapper.get(), *m_collisionSystem.get() };
		m_impulseSystem = std::make_unique<ImpulseSystem>(impulseSysDesc);

		MovementTicksDesc mvTicksDesc = { {m_logger}, *m_ecsWrapper.get(), m_app.getThreadPool() };
		m_moveTicks = std::make_unique<MovementTicks>(mvTicksDesc);

		ThrusterSystemDesc thrSysDesc = { {m_logger}, *m_ecsWrapper.get() };
		m_thrusterSystem = std::make_unique<ThrusterSystem>(thrSysDesc);

		LifetimeSystemDesc ltsDesc = { {m_logger}, *m_ecsWrapper.get() };
		m_lifetimeSystem = std::make_unique<LifetimeSystem>(ltsDesc);

		DamageSystemDesc damSysDesc = { {m_logger}, *m_ecsWrapper.get(), *m_collisionSystem.get() };
		m_damageSystem = std::make_unique<DamageSystem>(damSysDesc);

		ParticleSystemDesc partSysDesc = { {m_logger}, *m_ecsWrapper.get(), *m_camera.get(), m_app.getWindow() };
		m_particleSystem = std::make_unique<ParticleSystem>(partSysDesc);

		WeaponSystemDesc wepSysDesc = { {m_logger}, *m_ecsWrapper.get() };
		m_weaponSystem = std::make_unique<WeaponSystem>(wepSysDesc);

		DamperSystemDesc dampSysDesc = { {m_logger}, *m_ecsWrapper.get() };
		m_damperSystem = std::make_unique<DamperSystem>(dampSysDesc);

		MountFollowSystemDesc mountFolSysDesc = { {m_logger}, *m_ecsWrapper.get() };
		m_mountFollowSystem = std::make_unique<MountFollowSystem>(mountFolSysDesc);

		MountLifecycleSystemDesc mlsDesc{ {m_logger}, *m_ecsWrapper.get() };
		m_mountLifecycleSystem = std::make_unique<MountLifecycleSystem>(mlsDesc);

		AISystemDesc aiDesc{ {m_logger}, *m_ecsWrapper.get() };
		m_aiSystem = std::make_unique<AISystem>(aiDesc);

		// Ship stuff

		PartRegistryDesc partRegDesc = { {m_logger} };
		m_partRegistry = std::make_unique<PartRegistry>(partRegDesc);

		ShipFactoryDesc shipFactoryDesc = { {m_logger}, *m_ecsWrapper.get(), *m_partRegistry.get() };
		m_shipFactory = std::make_unique<ShipFactory>(shipFactoryDesc);

		PartRenderSystemDesc prsDesc = { {m_logger}, *m_ecsWrapper.get(), *m_renderer.get(), *m_camera.get(), m_app.getWindow(), *m_meshRegistry.get(), *m_partRegistry.get() };
		m_partRenderSystem = std::make_unique<PartRenderSystem>(prsDesc);

		Scheduler& scheduler = m_app.getScheduler();
		scheduler.registerFrameSystem(m_renderSystem.get());
		scheduler.registerFrameSystem(m_cameraController.get());
		//scheduler.registerFrameSystem(m_particleSystem.get());
		scheduler.registerFrameSystem(m_partRenderSystem.get());

		scheduler.registerSystem(m_aiSystem.get());
		scheduler.registerSystem(m_thrusterSystem.get());
		scheduler.registerSystem(m_collisionSystem.get());
		scheduler.registerSystem(m_impulseSystem.get());
		scheduler.registerSystem(m_moveTicks.get());
		scheduler.registerSystem(m_lifetimeSystem.get());
		scheduler.registerSystem(m_damageSystem.get());
		scheduler.registerSystem(m_weaponSystem.get());
		scheduler.registerSystem(m_damperSystem.get());
		scheduler.registerSystem(m_mountFollowSystem.get());
		scheduler.registerSystem(m_mountLifecycleSystem.get());

		// Command buffers process queued calls for entity and component create/destroy for parallelization
		scheduler.registerFlushCallback([this]() { m_lifetimeSystem->getCommandBuffer().flush(); });
		scheduler.registerFlushCallback([this]() { m_damageSystem->getCommandBuffer().flush(); });
		scheduler.registerFlushCallback([this]() { m_weaponSystem->getCommandBuffer().flush(); });
		scheduler.registerFlushCallback([this]() { m_mountLifecycleSystem->getCommandBuffer().flush(); });

		// Ordering constraints ensure certain systems are updated before others due to order dependency
		m_app.getJobController().addOrderingConstraint(m_collisionSystem.get(), m_moveTicks.get());
		m_app.getJobController().addOrderingConstraint(m_collisionSystem.get(), m_damageSystem.get());

		EngineLogInfo("GameLayer attached, game initialized successfully.");

		/*
		CoreSystemsTestDesc cstDesc{ {m_logger}, *m_ecsWrapper.get() };
		m_coreSystemsTest = std::make_unique<CoreSystemsTest>(cstDesc);
		m_coreSystemsTest->spawnAll();
		*/

		/*
		WeaponTestDesc wtDesc{ {m_logger}, *m_ecsWrapper.get() };
		m_weaponTest = std::make_unique<WeaponTest>(wtDesc);
		m_weaponTest->spawnFiringShip(20.0, 0.1f, 30.0f, 0.10f, 5.0f, 100.0f);
		*/

		/*
		DamperTestDesc dTestDesc{ {m_logger}, *m_ecsWrapper.get() };
		m_damperTest = std::make_unique<DamperTest>(dTestDesc);
		m_damperTest->spawnComparisonRow(5.0, 5.0f, 0.5f, 0.1f);
		*/

		/*
		PartVariantID hullVariant = m_partRegistry->registerVariant(PartVariant{
			.name = "Basic Hull", .category = PartCategory::Hull,
			.params = HullParams{ PartBaseStats{ 10.0f, 50.0f, 20.0f, 0.0f } }
			});
		PartVariantID engineVariant = m_partRegistry->registerVariant(PartVariant{
			.name = "Basic Engine", .category = PartCategory::Engine,
			.params = EngineParams{ PartBaseStats{ 5.0f, 20.0f, 5.0f, 0.0f }, 50.0f, 10.0f }
			});
		PartVariantID hardpointVariant = m_partRegistry->registerVariant(PartVariant{
			.name = "Small Hardpoint", .category = PartCategory::Hardpoint,
			.params = HardpointParams{ PartBaseStats{ 2.0f, 15.0f, 5.0f, 0.0f }, 1, 1 }
			});

		ShipGridDesc sgDesc{ {m_logger}, 50, 50 };
		ShipGrid grid(sgDesc);

		grid.tryPlacePart(2, 2, 44, 44, PartCategory::Hull, hullVariant);

		// Engines along the back edge, evenly spaced
		for (i32 y = 12; y < 38; y += 8) {
			grid.tryPlacePart(0, y, 2, 2, PartCategory::Engine, engineVariant);
		}

		// Hardpoints along the front edge and both sides
		for (i32 y = 4; y < 46; y += 6) {
			grid.forcePlacePart(44, y, 1, 1, PartCategory::Hardpoint, hardpointVariant);
		}
		for (i32 x = 4; x < 46; x += 10) {
			grid.forcePlacePart(x, 3, 1, 1, PartCategory::Hardpoint, hardpointVariant);
			grid.forcePlacePart(x, 44, 1, 1, PartCategory::Hardpoint, hardpointVariant);
		}

		EntityID testShip = m_shipFactory->bake(grid, Vector2double{ 0.0, 0.0 }, 0.0f);

		auto& thruster = m_ecsWrapper->getComponent<Thruster>(testShip);
		thruster.throttle = 0.3f;
		*/

		ShipCollisionTestDesc sctDesc{ {m_logger}, *m_ecsWrapper.get(), *m_shipFactory.get(), *m_partRegistry.get() };
		m_shipCollisionTest = std::make_unique<ShipCollisionTest>(sctDesc);

		m_shipCollisionTest->spawnTwoSidedBattle(1000, 15.0, 1.0, 1.0f, 4.0f, 0.005f, 5.0f, 2.0f, 3.0f);
		m_app.getScheduler().togglePause();

	}

	void GameLayer::onDetach()
	{
		EngineLogInfo("GameLayer detached, game shutting down...");
	}

	void GameLayer::onUpdate(d64 dt)
	{
		m_renderer->beginFrame();
		m_app.getScheduler().advance();
	}
}