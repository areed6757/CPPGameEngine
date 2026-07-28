#pragma once
#include <Game.h>

namespace Engine {
	class GameLayer : public Base, public Layer {
	public:
		explicit GameLayer(Application& app);
		~GameLayer() override;

		void onAttach() override;
		void onDetach() override;
		void onUpdate(d64 dt) override;

	private:
		Application& m_app;

		// IMPORTANT: unique_ptrs must be instantiated in priority order.
		// The first of these objects created will be destroyed LAST.
		std::unique_ptr<Camera> m_camera{};
		std::unique_ptr<CameraController> m_cameraController{};
		std::unique_ptr<Renderer> m_renderer{};

		std::unique_ptr<EntityRegister> m_entityRegister{};
		std::unique_ptr<GameECSWrapper> m_ecsWrapper{};

		std::unique_ptr<QuadTree> m_quadtree{};
		std::unique_ptr<CollisionSystem> m_collisionSystem{};
		std::unique_ptr<ImpulseSystem> m_impulseSystem{};
		std::unique_ptr<MovementTicks> m_moveTicks{};
		std::unique_ptr<ThrusterSystem> m_thrusterSystem{};
		std::unique_ptr<LifetimeSystem> m_lifetimeSystem{};
		std::unique_ptr<DamageSystem> m_damageSystem{};
		std::unique_ptr<ParticleSystem> m_particleSystem{};
		std::unique_ptr<WeaponSystem> m_weaponSystem{};
		std::unique_ptr<DamperSystem> m_damperSystem{};
		std::unique_ptr<MountFollowSystem> m_mountFollowSystem{};

		std::unique_ptr<PartRegistry> m_partRegistry{};
		std::unique_ptr<ShipFactory> m_shipFactory{};

		std::unique_ptr<TextureRegistry> m_textureRegistry{};
		std::unique_ptr<MeshRegistry> m_meshRegistry{};
		std::unique_ptr<RenderSystem> m_renderSystem{};
		std::unique_ptr<PartRenderSystem> m_partRenderSystem{};

		std::unique_ptr<CoreSystemsTest> m_coreSystemsTest{};
		std::unique_ptr<ThreadingStressTest> m_threadingStressTest{};
		std::unique_ptr<WeaponTest> m_weaponTest{};
		std::unique_ptr<DamperTest> m_damperTest{};
	};
}