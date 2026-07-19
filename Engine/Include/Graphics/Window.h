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

		[[nodiscard]] i32 getWidth() const noexcept;
		[[nodiscard]] i32 getHeight() const noexcept;

		bool shouldClose() const noexcept;
		GLFWwindow* get() const noexcept;

		static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

		InputHandler* getInputHandler() const noexcept { return m_inputHandler.get(); };


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
		
		static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

		std::unique_ptr<InputHandler> m_inputHandler;
		
		i32 m_width{};
		i32 m_height{};
	};
}