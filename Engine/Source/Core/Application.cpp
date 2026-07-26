#include <Core/Application.h>
#include <Core/Layer.h>

namespace Engine {
	Application::Application(const ApplicationDesc& desc) : 
		Base({ *std::make_unique<Logger>(desc.logLevel).release()}),
		m_loggerPtr(&m_logger),
		m_layerStack({m_logger})
	{
		GLFWDesc glfwDesc{ { m_logger } };
		m_glfwContext = std::make_unique<GLFWContext>(glfwDesc);

		WindowDesc windowDesc{ {m_logger}, desc.windowWidth, desc.windowHeight, desc.title, m_actionMap };
		m_window = std::make_unique<Window>(windowDesc);

		m_inputHandler = m_window->getInputHandler();

		GameClockDesc clockDesc = { {m_logger} };
		m_gameClock = std::make_unique<GameClock>(clockDesc);

		ThreadPoolDesc tpDesc{ {m_logger} };
		m_threadPool = std::make_unique<ThreadPool>(tpDesc);

		JobControllerDesc jcDesc{ {m_logger}, *m_threadPool.get() };
		m_jobController = std::make_unique<JobController>(jcDesc);

		SchedulerDesc schedulerDesc = { {m_logger}, *m_gameClock, *m_jobController.get() };
		m_scheduler = std::make_unique<Scheduler>(schedulerDesc);

		EngineLogInfo("Application created.")
	}
	Application::~Application()
	{
		EngineLogInfo("Application shutting down...");
	}
	void Application::run()
	{
		while (m_isRunning && !glfwWindowShouldClose(m_window->get())) {
			glfwPollEvents();

			if (m_inputHandler->wasEventActivated("pause")) {
				m_scheduler->togglePause();
			}

			for (Layer* layer : m_layerStack) {
				layer->onUpdate(m_gameClock->getDelta());
			}

			m_inputHandler->endFrame();
		}
	}
	void Application::pushLayer(Layer* layer)
	{
		m_layerStack.pushLayer(layer);
	}
	void Application::pushOverlay(Layer* layer)
	{
		m_layerStack.pushOverlay(layer);
	}
	Window& Application::getWindow() noexcept
	{
		return *m_window.get();
	}
	InputHandler& Application::getInputHandler() noexcept
	{
		return *m_inputHandler;
	}
	Scheduler& Application::getScheduler() noexcept
	{
		return *m_scheduler.get();
	}
	ThreadPool& Application::getThreadPool() noexcept
	{
		return *m_threadPool.get();
	}
	JobController& Application::getJobController() noexcept
	{
		return *m_jobController.get();
	}
	GameClock& Application::getGameClock() noexcept
	{
		return *m_gameClock.get();
	}
}