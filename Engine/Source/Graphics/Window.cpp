#include <Graphics/Window.h>

Engine::Window::Window(const WindowDesc& desc) : Base(desc.base){
    GLFWwindow* rawWindow(glfwCreateWindow(desc.windowWidth, desc.windowHeight, desc.title, NULL, NULL));
    if (!rawWindow) {
        EngineLogErrorAndThrow("GLFW window creation failed.");
    }

    m_window.reset(rawWindow);

    glfwMakeContextCurrent(m_window.get());

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        EngineLogErrorAndThrow("Glad initiation failed.");
    }

    if (!m_window.get()) {
        EngineLogErrorAndThrow("GLFW window creation failed.");
        glfwTerminate();
    }

    EngineLogInfo("GLFW window created.");

    glfwMakeContextCurrent(m_window.get());
}

Engine::Window::~Window()
{
    EngineLogInfo("Window closing...");
}

bool Engine::Window::shouldClose() const noexcept
{
    return glfwWindowShouldClose(m_window.get());
}

GLFWwindow* Engine::Window::get() const noexcept
{
    return m_window.get();
}
