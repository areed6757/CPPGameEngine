#pragma once
#include <Core/Common.h>
#define GLFW_INCLUDE_NONE
#include <ThirdParty/glad/glad.h>
#include <GLFW/glfw3.h>
#include <Input/ActionMap.h>

namespace Engine {
	class InputHandler final : public Base {
	public:
		explicit InputHandler(InputHandlerDesc& desc);
		~InputHandler();
			
		InputHandler(const InputHandler&) = delete;
		InputHandler& operator = (const InputHandler&) = delete;


	private:
		// Handle inputs
		ActionMap& m_actionMap;
		std::vector<bool> m_keyStates;

		friend class Window;
		void onKey(i32 key, i32 scancode, i32 action, i32 mods); // Called by Window that creates this handler

		bool isKeyDown(i32 key) const noexcept;
	};
}