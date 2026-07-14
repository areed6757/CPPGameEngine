#include <Graphics/Window.h>
#include <Input/InputHandler.h>

// Wrapper to handle creation and destruction of OpenGL windows via GLFW/glad
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

    glfwSwapInterval(1); // int frames to wait to swap buffers (basically vsync)

    // Input handling - all inputs to a window will be passed through
    InputHandlerDesc handleDesc{ BaseDesc{desc.base.logger}, desc.actionMap };
    m_inputHandler = std::make_unique<InputHandler>(handleDesc);

    if (!m_inputHandler.get()) {
        EngineLogErrorAndThrow("Input handler not created for Window.");
    }

    // Window decoration "thicknesses"
    i32 left, top, right, bottom;
    glfwGetWindowFrameSize(m_window.get(), &left, &top, &right, &bottom);

    // Frame buffer size, TODO: handle window resizing
    i32 width, height;
    glfwGetFramebufferSize(m_window.get(), &width, &height);
    glViewport(0, 0, width, height);

    // Static color render
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(m_window.get());

    glfwSetWindowUserPointer(m_window.get(), this);
    glfwSetKeyCallback(m_window.get(), Window::key_callback);

    EngineLogInfo("GLFW window created.");
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

// Static key callback function for input handling, prompts the InputHandler associated with this Window.
void Engine::Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (self && self->m_inputHandler) {
        self->m_inputHandler->onKey(key, scancode, action, mods);
    }
}