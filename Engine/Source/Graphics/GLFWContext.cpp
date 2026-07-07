#include <Graphics/GLFWContext.h>
#include <format>

Engine::GLFWContext::GLFWContext(const GLFWDesc& desc) : Base(desc.base), windowWidth(desc.windowWidth), windowHeight(desc.windowHeight), title(desc.title)
{
    s_instance = this;
    glfwSetErrorCallback(&GLFWContext::error_callback);

    if (!glfwInit()) {
        EngineLogErrorAndThrow("GLFWInit failed.");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    uniqueGLFWWindow window(glfwCreateWindow(windowWidth /2, windowHeight /2, title, NULL, NULL));

    if (!window.get()) {
        EngineLogErrorAndThrow("GLFW window creation failed.");
        glfwTerminate();
    }

    glfwMakeContextCurrent(window.get());
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);    

    EngineLogInfo("GLFW initialized.");
}

Engine::GLFWContext::~GLFWContext() {
    EngineLogInfo("GLFW terminating...");
    glfwTerminate();
    s_instance = nullptr;
}

void Engine::GLFWContext::error_callback(int error, const char* description) {
    if (s_instance) {
        std::string str = std::format("GLFW Error: {} - {}", error, description);
        s_instance->getLogger().log(Logger::LogLevel::Error, str.c_str());
    }
}