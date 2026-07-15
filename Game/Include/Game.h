#pragma once
#include <Core/Base.h>
#include <Core/Core.h>
#include <Graphics/GLFWContext.h>
#include <Graphics/Window.h>
#include <Input/InputHandler.h>
#include <Input/ActionMap.h>
#include <Utilities/GameClock.h>
#include <Utilities/Scheduler.h>
#include <Systems/GraphicsTicks.h>
#include <Systems/MovementTicks.h>
#include <ECS/EntityRegister.h>
#include <ECSWrapper.h>
#include <Components/Position.h>
#include <Physics/Vector3double.h>
#include <Test/EntityStressTest.h>
#include <Components/Movement.h>

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
		
		// Ticked Systems + clock/scheduler
		std::unique_ptr<GameClock> m_gameClock{};
		std::unique_ptr<Scheduler> m_scheduler{};
		std::unique_ptr<GraphicsTicks> m_gfxTicks{};
		std::unique_ptr<MovementTicks> m_moveTicks{};

		// ECS
		std::unique_ptr<EntityRegister> m_entityRegister{};
		std::unique_ptr<ECSWrapper> m_ecsWrapper{};
		InputHandler* m_inputHandler{};
		ActionMap m_actionMap{};
		bool m_isRunning{ true };
	};
}