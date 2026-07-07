#pragma once
#include <Core/Common.h>
#define GLFW_INCLUDE_NONE
#include <ThirdParty/glad/glad.h>
#include <GLFW/glfw3.h>

namespace Engine {
	class Window final : public Base {
	public:
		explicit Window(const WindowDesc& desc);
		~Window();

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		bool shouldClose() const noexcept;
		GLFWwindow* get() const noexcept;

	private:
		struct GLFWwindowDeleter {
			void operator()(GLFWwindow* window) const {
				if (window) {
					glfwDestroyWindow(window);
				}
			}
		};

		using UniqueGLFWWindow = std::unique_ptr <GLFWwindow, GLFWwindowDeleter>;

		UniqueGLFWWindow m_window{};
	};
}