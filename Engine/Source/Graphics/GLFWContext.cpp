#include <Graphics/GLFWContext.h>
#include <format>

Engine::GLFWContext::GLFWContext(const GLFWDesc& desc)
    : Base(desc.base)
{
    s_instance = this;
    glfwSetErrorCallback(&GLFWContext::error_callback);

    if (!glfwInit()) {
        EngineLogErrorAndThrow("GLFWInit failed.");
    }

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