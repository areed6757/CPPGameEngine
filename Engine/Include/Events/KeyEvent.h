#pragma once
#include <cstdint>
#include <string>

namespace Engine {
	class KeyPressedEvent : public Event {
	public:
		KeyPressedEvent(i32 keyCode, bool isRepeat = false) : m_keyCode(keyCode), m_isRepeat(isRepeat) {};

		[[nodiscard]] i32 getKeyCode() { return m_keyCode; }
		[[nodiscard]] bool isRepeat() { return m_isRepeat; }

		std::string toString() const override {
			return "KeyPressedEvent: " + std::to_string(m_keyCode) + (m_isRepeat ? " (repeat)" : "");
		}

		EVENT_CLASS_TYPE(KeyPressed)
		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

	private:
		i32 m_keyCode;
		bool m_isRepeat;
	};

	class KeyReleasedEvent : public Event {
	public:
		explicit KeyReleasedEvent(i32 keyCode) : m_keyCode(keyCode){}

		[[nodiscard]] i32 getKeyCode() const { return m_keyCode; }

		std::string toString() const override {
			return "KeyReleasedEvent: " + std::to_string(m_keyCode);
		}

		EVENT_CLASS_TYPE(KeyReleased)
		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

	private:
		i32 m_keyCode;
	};
}