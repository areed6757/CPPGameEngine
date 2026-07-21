#include <Graphics/GLFWContext.h>
#include <format>

// Basic context hanlder for GLFW, extends error_callbacks into the global logger
Engine::GLFWContext::GLFWContext(const GLFWDesc& desc)
    : Base(desc.base)
{
    s_instance = this;
    glfwSetErrorCallback(&GLFWContext::error_callback);

    if (!glfwInit()) {
        EngineLogErrorAndThrow("GLFWInit failed.");
    }

    // GLFW Version declaration
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);

    // CORE profile is the modern OpenGL library only
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    EngineLogInfo("GLFW initialized.");
}

Engine::GLFWContext::~GLFWContext() {
    EngineLogInfo("GLFW terminating...");
    glfwTerminate();
    s_instance = nullptr;
}

void Engine::GLFWContext::error_callback(int error, const char* description) {
    if (s_instance) {
        s_instance->getLogger().log(Logger::LogLevel::Error, "GLFW Error: {} - {}", error, description);
    }
}