#pragma once
#include <stdexcept>
#include <memory>

// Forward declarations and common typedefs
namespace Engine {
	class Base;
	class Logger;
	class Game;
	class GLFWContext;
	class Window;
	
	using i32 = std::int32_t;
	using ui32 = std::uint32_t;
	using f32 = float;
	using d64 = double;
}