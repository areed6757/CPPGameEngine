#pragma once
#include <Core/Base.h>
#include <Core/Core.h>
#include <Graphics/GLFWContext.h>
#include <Graphics/Window.h>
#include <Input/InputHandler.h>
#include <Input/ActionMap.h>
#include <Utilities/GameClock.h>
#include <Utilities/Scheduler.h>
#include <Systems/MovementTicks.h>
#include <ECS/EntityRegister.h>
#include <ECS/ECSWrapper.h>
#include <Components/Position.h>
#include <Physics/Vector3double.h>
#include <Test/EntityStressTest.h>
#include <Components/Movement.h>
#include <Graphics/Renderer.h>
#include <GameDescs.h>
#include <GameECS.h>
#include <Graphics/Mesh.h>
#include <Graphics/Texture.h>
#include <Graphics/TextureRegistry.h>
#include <Graphics/TextureID.h>
#include <Graphics/MeshRegistry.h>
#include <Graphics/MeshID.h>
#include <Systems/RenderSystem.h>
#include <Graphics/Camera.h>
#include <Systems/CameraController.h>
#include <Test/RenderGridTest.h>
#include <Utilities/QuadTree.h>
#include <Systems/QuadtreeDebugSystem.h>
#include <Systems/CollisionSystem.h>
#include <Graphics/DebugLineRenderer.h>
#include <Test/CollisionTest.h>
#include <Systems/ThrusterSystem.h>
#include <Test/ThrusterTest.h>
#include <Systems/LifetimeSystem.h>
#include <Systems/DamageSystem.h>
#include <Test/DamageTest.h>

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
		std::unique_ptr<RenderGridTest> m_gridTest{};
		std::unique_ptr<CollisionTest> m_collisionTest{};
		std::unique_ptr<ThrusterTest> m_thrusterTest{};
		std::unique_ptr<DamageTest> m_damageTest{};


		InputHandler* m_inputHandler{};
		ActionMap m_actionMap{};
		bool m_isRunning{ true };
	};
}