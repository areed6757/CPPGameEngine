#pragma once
#include <Core/Common.h>
#define GLFW_INCLUDE_NONE
#include <ThirdParty/glad/glad.h>
#include <GLFW/glfw3.h>
#include <Input/ActionMap.h>
#include <utility>

namespace Engine {
	struct KeyState {
		bool isDown{ false };
		bool wasDownLastFrame{ false };
	};

	class InputHandler final : public Base {
	public:
		explicit InputHandler(InputHandlerDesc& desc);
		~InputHandler();
			
		InputHandler(const InputHandler&) = delete;
		InputHandler& operator = (const InputHandler&) = delete;

		bool wasEventActivated(std::string_view eventName) const;

		void endFrame() noexcept;

		bool isKeyDown(i32 key) const noexcept;
		bool wasKeyJustPressed(i32 key) const noexcept;

	private:
		// Handle inputs
		ActionMap& m_actionMap;
		std::vector<KeyState> m_keyStates;
		std::vector<std::pair<std::string, KeyState>> m_eventStates;

		friend class Window;
		void onKey(i32 key, i32 scancode, i32 action, i32 mods); // Called by Window that creates this handler
	};
}