#pragma once
#include <Core/Base.h>
#include <Core/Core.h>
#include <ECS/EntityRegister.h>
#include <ECS/ECSWrapper.h>
#include <Graphics/Camera.h>
#include <Graphics/DebugLineRenderer.h>
#include <Graphics/GLFWContext.h>
#include <Graphics/Mesh.h>
#include <Graphics/MeshID.h>
#include <Graphics/MeshRegistry.h>
#include <Graphics/Renderer.h>
#include <Graphics/Texture.h>
#include <Graphics/TextureRegistry.h>
#include <Graphics/TextureID.h>
#include <Graphics/Window.h>
#include <Physics/Vector3double.h>
#include <Input/ActionMap.h>
#include <Input/InputHandler.h>
#include <Systems/CameraController.h>
#include <Systems/CollisionSystem.h>
#include <Systems/DamageSystem.h>
#include <Systems/LifetimeSystem.h>
#include <Systems/MovementTicks.h>
#include <Systems/QuadtreeDebugSystem.h>
#include <Systems/RenderSystem.h>
#include <Systems/ThrusterSystem.h>
#include <Test/CoreSystemsTest.h>
#include <Utilities/GameClock.h>
#include <Utilities/QuadTree.h>
#include <Utilities/Scheduler.h>
#include <Utilities/ThreadPool.h>
#include <Utilities/JobController.h>
#include <Test/ThreadingStressTest.h>

namespace Engine {
	class Game : public Base {
	public:
		explicit Game(const GameDesc& desc);
		virtual ~Game() override;

		virtual void run() final;


		// IMPORTANT: unique_ptrs must be instantiated in priority order.
		// The first of these objects created will be destroyed LAST.
	private:

		// Core startup
		std::unique_ptr<Logger> m_loggerPtr{};
		std::unique_ptr<GLFWContext> m_glfwContext{};
		std::unique_ptr<Window> m_window{};
		std::unique_ptr<Camera> m_camera{};
		std::unique_ptr<CameraController> m_cameraController{};
		std::unique_ptr<Renderer> m_renderer{};
		std::unique_ptr<ThreadPool> m_threadPool{};
		std::unique_ptr<JobController> m_jobController{};

		// ECS
		std::unique_ptr<EntityRegister> m_entityRegister{};
		std::unique_ptr<GameECSWrapper> m_ecsWrapper{};

		// Ticked Systems + clock/scheduler + quadtree
		std::unique_ptr<GameClock> m_gameClock{};
		std::unique_ptr<Scheduler> m_scheduler{};
		std::unique_ptr<QuadTree> m_quadtree{};
		std::unique_ptr<DebugLineRenderer> m_debugLineRenderer{};
		std::unique_ptr<QuadtreeDebugSystem> m_quadtreeDebugSystem{};
		std::unique_ptr<CollisionSystem> m_collisionSystem{};
		std::unique_ptr<MovementTicks> m_moveTicks{};
		std::unique_ptr<ThrusterSystem> m_thrusterSystem{};
		std::unique_ptr<LifetimeSystem> m_lifetimeSystem{};
		std::unique_ptr<DamageSystem> m_damageSystem{};

		// Graphics
		std::unique_ptr<TextureRegistry> m_textureRegistry{};
		std::unique_ptr<MeshRegistry> m_meshRegistry{};
		std::unique_ptr<RenderSystem> m_renderSystem{};

		// Test Systems
		std::unique_ptr<CoreSystemsTest> m_coreSystemsTest{};
		std::unique_ptr<ThreadingStressTest> m_threadingStressTest{};


		InputHandler* m_inputHandler{};
		ActionMap m_actionMap{};
		bool m_isRunning{ true };
	};
}