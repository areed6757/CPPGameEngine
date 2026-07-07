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

    protected:
        i32 windowHeight{};
        i32 windowWidth {};
        const char* title{};


    private:
        static inline GLFWContext* s_instance = nullptr;

        static void error_callback(int error, const char* description);
        
        struct GLFWwindowDeleter {
            void operator()(GLFWwindow* window) const {
                if (window) {
                    glfwDestroyWindow(window);
                }
            }
        };

        using uniqueGLFWWindow = std::unique_ptr <GLFWwindow, GLFWwindowDeleter>;
    };
}