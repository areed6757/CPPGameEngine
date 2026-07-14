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
	class EntityRegister;
	class ECSWrapper;
	class EntityStressTest;

	class TickedSystem;
	class MovementTicks;

	struct ActionMap;
	struct KeyStates;
	struct Vector3double;
	struct Vector3float;
	struct EntityID;
	struct ComponentPools;

	struct Movement;
	struct Position;
	
	using i32 = std::int32_t;
	using i64 = std::int64_t;
	using ui32 = std::uint32_t;
	using f32 = float;
	using d64 = double;
}