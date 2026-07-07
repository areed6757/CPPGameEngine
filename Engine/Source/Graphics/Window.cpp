#include <Graphics/Window.h>

Engine::Window::Window(const WindowDesc& desc) : Base(desc.base), windowWidth(desc.windowWidth), windowHeight(desc.windowHeight), title(desc.title)
{
    uniqueGLFWWindow window(glfwCreateWindow(windowWidth / 2, windowHeight / 2, title, NULL, NULL));

    if (!window.get()) {
        EngineLogErrorAndThrow("GLFW window creation failed.");
        glfwTerminate();
    }

    glfwMakeContextCurrent(window.get());
}

Engine::Window::~Window()
{
}
