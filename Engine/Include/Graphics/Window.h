#pragma once
#include <Core/Common.h>
#define GLFW_INCLUDE_NONE
#include <ThirdParty/glad/glad.h>
#include <GLFW/glfw3.h>
#include <Events/Event.h>
#include <Events/KeyEvent.h>

namespace Engine {
	struct WindowDesc {
		BaseDesc base;

		// Default window fields set by instantiation in GameDesc
		i32 windowWidth = {};
		i32 windowHeight = {};
		const char* title = {};
	};

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

		void setEventCallback(std::function<void(Event&)> callback) { m_eventCallback = std::move(callback); }

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

		static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		void onKeyEvent(int key, int action, bool isRepeat);

		std::function<void(Event&)> m_eventCallback;
		
		i32 m_width{};
		i32 m_height{};
	};
}