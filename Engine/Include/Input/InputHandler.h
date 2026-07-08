#pragma once
#include <Core/Common.h>
#define GLFW_INCLUDE_NONE
#include <ThirdParty/glad/glad.h>
#include <GLFW/glfw3.h>

namespace Engine {
	class InputHandler final : public Base {
	public:
		explicit InputHandler(InputHandlerDesc& desc);
		~InputHandler();
			
		InputHandler(const InputHandler&) = delete;
		InputHandler& operator = (const InputHandler&) = delete;

		void onKey(i32 key, i32 scancode, i32 action, i32 mods); // Called by Window that creates this handler

	private:
		// Handle inputs
		ActionBindings& m_bindings;
		std::vector<std::pair<i32, bool>> m_keyStates;


	};
}