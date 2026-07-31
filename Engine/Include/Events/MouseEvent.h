#pragma once
#include <string>

namespace Engine {
	class MouseMovedEvent : public Event {
	public:
		MouseMovedEvent(d64 x, d64 y) : m_x(x), m_y(y) {}

		[[nodiscard]] d64 getX() const { return m_x; }
		[[nodiscard]] d64 getY() const { return m_y; }

		std::string toString() const override {
			return "MouseMovedEvent: " + std::to_string(m_x) + ", " + std::to_string(m_y);
		}

		EVENT_CLASS_TYPE(MouseMoved)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

	private:
		d64 m_x, m_y;
	};

	class MouseScrolledEvent : public Event {
	public:
		MouseScrolledEvent(d64 xOffset, d64 yOffset) : m_xOffset(xOffset), m_yOffset(yOffset) {}

		[[nodiscard]] d64 getXOffset() const { return m_xOffset; }
		[[nodiscard]] d64 getYOffset() const { return m_yOffset; }

		std::string toString() const override {
			return "MouseScrolledEvent: " + std::to_string(m_xOffset) + ", " + std::to_string(m_yOffset);
		}

		EVENT_CLASS_TYPE(MouseScrolled)
			EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

	private:
		d64 m_xOffset, m_yOffset;
	};

	class MouseButtonPressedEvent : public Event {
	public:
		explicit MouseButtonPressedEvent(i32 button) : m_button(button) {}

		[[nodiscard]] i32 getButton() const { return m_button; }

		std::string toString() const override {
			return "MouseButtonPressedEvent: " + std::to_string(m_button);
		}

		EVENT_CLASS_TYPE(MouseButtonPressed)
			EVENT_CLASS_CATEGORY(EventCategoryMouseButton | EventCategoryMouse | EventCategoryInput)

	private:
		i32 m_button;
	};

	class MouseButtonReleasedEvent : public Event {
	public:
		explicit MouseButtonReleasedEvent(i32 button) : m_button(button) {}

		[[nodiscard]] i32 getButton() const { return m_button; }

		std::string toString() const override {
			return "MouseButtonReleasedEvent: " + std::to_string(m_button);
		}

		EVENT_CLASS_TYPE(MouseButtonReleased)
			EVENT_CLASS_CATEGORY(EventCategoryMouseButton | EventCategoryMouse | EventCategoryInput)

	private:
		i32 m_button;
	};
}