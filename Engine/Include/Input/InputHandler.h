#pragma once
#include <Core/Common.h>
#define GLFW_INCLUDE_NONE
#include <ThirdParty/glad/glad.h>
#include <GLFW/glfw3.h>
#include <Input/ActionMap.h>
#include <utility>

namespace Engine {
	struct InputHandlerDesc {
		BaseDesc base;
		ActionMap& actionMap;
	};

	struct KeyState {
		bool isDown{ false };
		bool wasDownLastFrame{ false };
	};

	class InputHandler final : public Base {
	public:
		explicit InputHandler(const InputHandlerDesc& desc);
		~InputHandler();
			
		InputHandler(const InputHandler&) = delete;
		InputHandler& operator = (const InputHandler&) = delete;

		bool wasEventActivated(ActionID action) const;
		void endFrame() noexcept;

		bool isKeyDown(i32 key) const noexcept;
		[[nodiscard]] bool isKeyDown(ActionID action) const noexcept;

		bool wasKeyJustPressed(i32 key) const noexcept;
		bool wasKeyJustReleased(i32 key) const noexcept;

		[[nodiscard]] bool isMouseButtonDown(i32 button) const noexcept;
		bool wasMouseButtonJustPressed(i32 button) const noexcept;

		[[nodiscard]] Vector2double getMousePosition() const noexcept { return m_mousePos; }
		[[nodiscard]] Vector2double getMouseDelta() const noexcept { return m_mouseDelta; }
		[[nodiscard]] f32 getScrollDelta() const noexcept { return m_scrollDelta; }


	private:
		ActionMap& m_actionMap;
		std::vector<KeyState> m_keyStates;
		std::vector<KeyState> m_mouseButtonStates;
		Vector2double m_mousePos{};
		Vector2double m_lastMousePos{};
		Vector2double m_mouseDelta{};
		f32 m_scrollDelta{ 0.0f };

		friend class Application;
		void onKey(i32 key, i32 scancode, i32 action, i32 mods);
		void onMouseButton(i32 button, i32 action, i32 mods);
		void onMouseMove(d64 x, d64 y);
		void onScroll(d64 xOffset, d64 yOffset);

	};
}