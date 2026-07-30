#include <Graphics/Window.h>
#include <Input/InputHandler.h>

// Wrapper to handle creation and destruction of OpenGL windows via GLFW/glad
namespace Engine {
    Window::Window(const WindowDesc& desc) : Base(desc.base) {
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

        // Window decoration "thicknesses"
        i32 left, top, right, bottom;
        glfwGetWindowFrameSize(m_window.get(), &left, &top, &right, &bottom);

        i32 width, height;
        glfwGetFramebufferSize(m_window.get(), &width, &height);
        m_width = width;
        m_height = height;
        glViewport(0, 0, width, height); // From x = 0, y = 0 -> x = width, y = height

        glfwSetWindowUserPointer(m_window.get(), this);
        glfwSetKeyCallback(m_window.get(), &Window::keyCallback);
        glfwSetMouseButtonCallback(m_window.get(), &Window::mouseButtonCallback);
        glfwSetCursorPosCallback(m_window.get(), &Window::cursorPosCallback);
        glfwSetScrollCallback(m_window.get(), &Window::scrollCallback);
        glfwSetFramebufferSizeCallback(m_window.get(), Window::framebuffer_size_callback);

        EngineLogInfo("GLFW window created.");
    }

    Window::~Window()
    {
        EngineLogInfo("Window closing...");
    }

    bool Window::shouldClose() const noexcept
    {
        return glfwWindowShouldClose(m_window.get());
    }

    GLFWwindow* Window::get() const noexcept
    {
        return m_window.get();
    }

    void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
        if (action == GLFW_PRESS) { self->onKeyEvent(key, action, false); }
        else if (action == GLFW_REPEAT) { self->onKeyEvent(key, action, true); }
        else if (action == GLFW_RELEASE) { self->onKeyEvent(key, action, false); }
    }

    void Window::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
    {
        Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
        self->onMouseButtonEvent(button, action);
    }

    void Window::cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
    {
        Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
        self->onCursorPosEvent(xpos, ypos);
    }

    void Window::scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
    {
        Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
        self->onScrollEvent(xoffset, yoffset);
    }

    void Window::onKeyEvent(int key, int action, bool isRepeat) {
        if (!m_eventCallback) { return; }
        if (action == GLFW_RELEASE) {
            KeyReleasedEvent event(key);
            m_eventCallback(event);
        }
        else {
            KeyPressedEvent event(key, isRepeat);
            m_eventCallback(event);
        }
    }

    void Window::onMouseButtonEvent(int button, int action)
    {
        if (!m_eventCallback) { return; }
        if (action == GLFW_PRESS) {
            MouseButtonPressedEvent event(button);
            m_eventCallback(event);
        }
        else if (action == GLFW_RELEASE) {
            MouseButtonReleasedEvent event(button);
            m_eventCallback(event);
        }
    }

    void Window::onCursorPosEvent(double xpos, double ypos)
    {
        if (!m_eventCallback) { return; }
        MouseMovedEvent event(xpos, ypos);
        m_eventCallback(event);
    }

    void Window::onScrollEvent(double xoffset, double yoffset)
    {
        if (!m_eventCallback) { return; }
        MouseScrolledEvent event(xoffset, yoffset);
        m_eventCallback(event);
    }

    void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
        auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
        if (self) { self->m_width = width; self->m_height = height; }
    }

    i32 Window::getHeight() const noexcept { return m_height; }
    i32 Window::getWidth() const noexcept { return m_width; }
}