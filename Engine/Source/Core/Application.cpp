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

		InputHandlerDesc inputHandlerDesc{ { m_logger}, m_actionMap };
		m_inputHandler = std::make_unique<InputHandler>(inputHandlerDesc);

		WindowDesc windowDesc{ {m_logger}, desc.windowWidth, desc.windowHeight, desc.title };
		m_window = std::make_unique<Window>(windowDesc);
		m_window->setEventCallback([this](Event& e) { onEvent(e); });

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

			if (m_inputHandler->wasEventActivated(ActionID::Pause)) {
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

	void Application::onEvent(Event& e)
	{
		if (e.getEventType() == EventType::KeyPressed) {
			auto& kp = static_cast<KeyPressedEvent&>(e);
			m_inputHandler->onKey(kp.getKeyCode(), 0, kp.isRepeat() ? GLFW_REPEAT : GLFW_PRESS, 0);
		}
		else if (e.getEventType() == EventType::KeyReleased) {
			auto& kr = static_cast<KeyReleasedEvent&>(e);
			m_inputHandler->onKey(kr.getKeyCode(), 0, GLFW_RELEASE, 0);
		}
		else if (e.getEventType() == EventType::MouseButtonPressed) {
			auto& mp = static_cast<MouseButtonPressedEvent&>(e);
			m_inputHandler->onMouseButton(mp.getButton(), GLFW_PRESS, 0);
		}
		else if (e.getEventType() == EventType::MouseButtonReleased) {
			auto& mr = static_cast<MouseButtonReleasedEvent&>(e);
			m_inputHandler->onMouseButton(mr.getButton(), GLFW_RELEASE, 0);
		}
		else if (e.getEventType() == EventType::MouseMoved) {
			auto& mm = static_cast<MouseMovedEvent&>(e);
			m_inputHandler->onMouseMove(mm.getX(), mm.getY());
		}
		else if (e.getEventType() == EventType::MouseScrolled) {
			auto& ms = static_cast<MouseScrolledEvent&>(e);
			m_inputHandler->onScroll(ms.getXOffset(), ms.getYOffset());
		}

		for (auto it = m_layerStack.rbegin(); it != m_layerStack.rend(); ++it) {
			if (e.handled) { break; }
			(*it)->onEvent(e);
		}
	}
}