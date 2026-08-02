#include <cstdlib>
#include <Core/Application.h>
#include <GameLayer.h>
#include <GUI/ImGUILayer.h>

int main()
{
	try {
		Engine::ApplicationDesc desc{};
		Engine::Application app(desc);

		Engine::ImGUILayer* imguiLayer = new Engine::ImGUILayer(app);
		Engine::GameLayer* gameLayer = new Engine::GameLayer(app, *imguiLayer);

		app.pushLayer(gameLayer);
		app.pushOverlay(imguiLayer);
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