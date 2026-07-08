#pragma once
#include <Core/Common.h>

namespace Engine {
	struct ActionBindings {
		std::vector<std::pair<i32, std::string>> bindings = {
			{GLFW_KEY_W, "MoveForward"},
			{GLFW_KEY_A, "MoveLeft"},
			{GLFW_KEY_S, "MoveBackward"},
			{GLFW_KEY_D, "MoveRight"}
		};


	};
}