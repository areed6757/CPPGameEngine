#pragma once
#define GLFW_INCLUDE_NONE
#include <ThirdParty/glad/glad.h>
#include <Core/Common.h>
#include <GLFW/glfw3.h>

namespace Engine {
    class GLFWContext : public Base {
    public:
        explicit GLFWContext(const GLFWDesc& desc);
        ~GLFWContext();

        GLFWContext(const GLFWContext&) = delete;
        GLFWContext& operator=(const GLFWContext&) = delete;

    private:
        static inline GLFWContext* s_instance = nullptr;

        static void error_callback(int error, const char* description);
    };
}