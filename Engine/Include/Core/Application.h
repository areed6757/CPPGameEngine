#pragma once
#include <Core/Common.h>
#include <Core/LayerStack.h>
#include <Graphics/GLFWContext.h>
#include <Graphics/Window.h>
#include <Input/InputHandler.h>
#include <Input/ActionMap.h>
#include <Utilities/GameClock.h>
#include <Utilities/JobController.h>
#include <Utilities/ThreadPool.h>
#include <Utilities/Scheduler.h>
#include <memory>

namespace Engine {
	struct ApplicationDesc {
		Logger::LogLevel logLevel = LOG_LEVEL;
		i32 windowWidth = WINDOW_WIDTH;
		i32 windowHeight = WINDOW_HEIGHT;
		const char* title = TITLE;
		d64 tickRate = TICK_RATE;
	};

	class Application : public Base {
	public :
		explicit Application(const ApplicationDesc& desc);
		virtual ~Application();

		void run();
		void pushLayer(Layer* layer);
		void pushOverlay(Layer* layer);

		Logger& getLogger() noexcept { return m_logger; }

		Window& getWindow() noexcept;
		InputHandler& getInputHandler() noexcept;
		Scheduler& getScheduler() noexcept;
		ThreadPool& getThreadPool() noexcept;
		JobController& getJobController() noexcept;
		GameClock& getGameClock() noexcept;

	private:
		std::unique_ptr<Logger> m_loggerPtr{};
		std::unique_ptr<GLFWContext> m_glfwContext;
		std::unique_ptr<Window> m_window;
		InputHandler* m_inputHandler;
		ActionMap m_actionMap;
		std::unique_ptr<GameClock> m_gameClock;
		std::unique_ptr<ThreadPool> m_threadPool;
		std::unique_ptr<JobController> m_jobController;
		std::unique_ptr<Scheduler> m_scheduler;

		LayerStack m_layerStack;
		bool m_isRunning{ true };

	};
}