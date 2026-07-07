#include <Game/Game.h>
#include <Core/Logger.h>

/*
* Game handles creation and destruction of the base client and owns the single Logger referenced by other classes.
* The Base class is constructed with {} to pass the BaseDesc (descriptor), and requires a non-null reference to the
* Logger. The Logger instance is then detatched from the unique ptr via .release() and then dereferenced with *.
*/
Engine::Game::Game(const GameDesc& desc) :
	Base({ *std::make_unique<Logger>(desc.logLevel).release() }),
	m_loggerPtr(&m_logger)
{
	GLFWDesc glfwDesc{ BaseDesc{ m_logger } };
	m_glfwContext = std::make_unique<GLFWContext>(glfwDesc);
	WindowDesc windowDesc{ BaseDesc {m_logger} };
	m_window = std::make_unique<Window>(WindowDesc{ {m_logger}, desc.windowWidth, desc.windowHeight, desc.title });
	EngineLogInfo("Game Initialized.");
	/*
	if (glfwWindowShouldClose(m_window->get())) {
		m_window.get()->shouldClose();
	}
	*/
}

Engine::Game::~Game()
{
	EngineLogInfo("Game shutting down...");
}

void Engine::Game::run()
{
}
