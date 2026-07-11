#include <Game/Game.h>
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
	GLFWDesc glfwDesc{ { m_logger } };
	m_glfwContext = std::make_unique<GLFWContext>(glfwDesc);
	WindowDesc windowDesc{ {m_logger}, desc.windowWidth, desc.windowHeight, desc.title, m_actionMap };
	m_window = std::make_unique<Window>(windowDesc);
	m_inputHandler = m_window->getInputHandler();

	GameClockDesc clockDesc = { {m_logger} };
	m_gameClock = std::make_unique<GameClock>(clockDesc);
	if (!m_gameClock) { EngineLogErrorAndThrow("GameClock failed to initialize.") };

	SchedulerDesc schedulerDesc = { {m_logger}, *m_gameClock };
	m_scheduler = std::make_unique<Scheduler>(schedulerDesc);
	if (!m_scheduler) { EngineLogErrorAndThrow("Scheduler failed to initialize.") };

	GraphicsTicksDesc gfxTicksDesc = { {m_logger} };
	m_gfxTicks = std::make_unique<GraphicsTicks>(gfxTicksDesc);
	if (!m_gfxTicks) { EngineLogErrorAndThrow("GraphicsTicks failed to initialize.") };
	m_scheduler->registerSystem(m_gfxTicks.get());

	EntityRegisterDesc eRegDesc = { {m_logger} };
	m_entityRegister = std::make_unique<EntityRegister>(eRegDesc);
	if (!m_entityRegister) { EngineLogErrorAndThrow("EntityRegister failed to initialize.") }

	EngineLogInfo("Game initialized successfully.");
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

		m_scheduler.get()->advance();

		m_inputHandler->endFrame();
	}
}