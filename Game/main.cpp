#include <cstdlib>
#include <Core/Application.h>
#include <GameLayer.h>

int main()
{
	try {
		Engine::ApplicationDesc desc{};
		Engine::Application app(desc);
		app.pushLayer(new Engine::GameLayer(app));
		app.run();		
	}
	catch (const std::runtime_error&) {
		return EXIT_FAILURE;
	}
	catch (const std::exception&) {
		return EXIT_FAILURE;
	}
	catch (...) {
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}