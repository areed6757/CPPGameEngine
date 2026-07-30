#pragma once
#include <cstdint>

namespace Engine {
	enum class EventType {
		None = 0,
		WindowResize, WindowClose,
		KeyPressed, KeyReleased,
		MouseMoved, MouseScrolled, MouseButtonPressed, MouseButtonReleased,
		Count
	};

	enum EventCategory : uint32_t {
		None = 0,
		EventCategoryApp = 1 << 0,
		EventCategoryInput = 1 << 1,
		EventCategoryKeyboard = 1 << 2,
		EventCategoryMouse = 1 << 3,
		EventCategoryMouseButton = 1 << 4
	};

	class Event {
	public:
		virtual ~Event() = default;

		[[nodiscard]] virtual EventType getEventType() const = 0;
		[[nodiscard]] virtual uint32_t getCategoryFlags() const = 0;
		[[nodiscard]] virtual std::string toString() const { return "Event"; }

		[[nodiscard]] bool isInCategory(EventCategory category) const { return getCategoryFlags() & category; }

		bool handled = false;
	};


	// Reused boilerplate
#define EVENT_CLASS_TYPE(type) \
	static EventType getStaticType() { return EventType::type; } \
	EventType getEventType() const override { return getStaticType(); }

#define EVENT_CLASS_CATEGORY(category) \
	uint32_t getCategoryFlags() const override { return category; }
}