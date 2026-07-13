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
	class InputHandler;
	class GameClock;
	class Scheduler;
	class TickedSystem;
	class EntityRegister;
	class ECSWrapper;
	class EntityStressTest;

	struct ActionMap;
	struct KeyStates;
	struct Vector3double;
	struct Vector3float;
	struct EntityID;
	struct ComponentPools;
	
	using i32 = std::int32_t;
	using i64 = std::int64_t;
	using ui32 = std::uint32_t;
	using f32 = float;
	using d64 = double;
}