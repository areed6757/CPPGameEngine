#include <Input/InputHandler.h>
#include <format>

namespace Engine {
	InputHandler::InputHandler(const InputHandlerDesc& desc) : Base(desc.base),
		m_actionMap(desc.actionMap), 
		m_keyStates(GLFW_KEY_LAST + 1, { false, false }),
		m_mouseButtonStates(GLFW_MOUSE_BUTTON_LAST + 1, {false, false})
	{
		m_actionMap.bindings.shrink_to_fit();
		EngineLogInfo("InputHandler created.");
	}

	InputHandler::~InputHandler()
	{
		EngineLogInfo("InputHandler destroyed.");
	}

	bool InputHandler::wasEventActivated(ActionID action) const
	{
		for (const auto& [key, boundAction] : m_actionMap.bindings) {
			if (boundAction == action) { return wasKeyJustPressed(key); }
		}
		EngineLogError("wasEventActivated in InputHandler called with invalid actionID");
		return false;
	}

	void InputHandler::endFrame() noexcept
	{
		for (auto& state : m_keyStates) { state.wasDownLastFrame = state.isDown; }
		for (auto& state : m_mouseButtonStates) { state.wasDownLastFrame = state.isDown; }
		m_mouseDelta = Vector2double{};
		m_scrollDelta = 0.0f;
	}

	void InputHandler::onKey(i32 key, i32 scancode, i32 action, i32 mods)
	{
		if (key < 0 || key >= static_cast<i32>(m_keyStates.size())) { return; }

		switch (action) {
		case GLFW_PRESS:
			m_keyStates[key].isDown = true;
			return;
		case GLFW_RELEASE:
			m_keyStates[key].isDown = false;
			return;
		case GLFW_REPEAT:
			return;
		default:
			EngineLogWarning("Unexpected GLFW key action.");
			return;
		}
	}

	void InputHandler::onMouseButton(i32 button, i32 action, i32 mods)
	{
		if (button < 0 || button >= static_cast<i32>(m_mouseButtonStates.size())) { return; }
		if (action == GLFW_PRESS) { m_mouseButtonStates[button].isDown = true; }
		else if (action == GLFW_RELEASE) { m_mouseButtonStates[button].isDown = false; }
	}

	void InputHandler::onMouseMove(d64 x, d64 y)
	{
		m_mousePos = Vector2double{ x, y };
		m_mouseDelta = m_mousePos - m_lastMousePos;
		m_lastMousePos = m_mousePos;
	}

	void InputHandler::onScroll(d64 xOffset, d64 yOffset)
	{
		m_scrollDelta += static_cast<f32>(yOffset);
	}

	bool InputHandler::isKeyDown(i32 key) const noexcept
	{
		if (key < 0 || key >= static_cast<i32>(m_keyStates.size())) { return false; }
		return m_keyStates[key].isDown;
	}

	bool InputHandler::isKeyDown(ActionID action) const noexcept
	{
		for (const auto& [key, boundAction] : m_actionMap.bindings) {
			if (boundAction == action) { return isKeyDown(key); }
		}
		EngineLogError("isKeyDown in InputHandler called with unbound ActionID");
		return false;
	}

	bool InputHandler::wasKeyJustPressed(i32 key) const noexcept
	{
		if (key < 0 || key >= static_cast<i32>(m_keyStates.size())) { return false; }
		return !m_keyStates[key].wasDownLastFrame && m_keyStates[key].isDown;
	}
	bool InputHandler::wasKeyJustReleased(i32 key) const noexcept
	{
		if (key < 0 || key >= static_cast<i32>(m_keyStates.size())) { return false; }
		return m_keyStates[key].wasDownLastFrame && !m_keyStates[key].isDown;
	}
	bool InputHandler::isMouseButtonDown(i32 button) const noexcept
	{
		if (button < 0 || button >= static_cast<i32>(m_mouseButtonStates.size())) { return false; }
		return m_mouseButtonStates[button].isDown;
	}
	bool InputHandler::wasMouseButtonJustPressed(i32 button) const noexcept
	{
		if (button < 0 || button >= static_cast<i32>(m_mouseButtonStates.size())) { return false; }
		return !m_mouseButtonStates[button].wasDownLastFrame && m_mouseButtonStates[button].isDown;
	}
}