#pragma once
#include <Core/Common.h>
#include <GLFW/glfw3.h>

namespace Engine {
	class Window : public Base {
	public:
		explicit Window(const WindowDesc& desc);
		~Window();

	protected:
		i32 windowHeight{};
		i32 windowWidth{};
		const char* title{};

	private:
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