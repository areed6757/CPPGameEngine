#pragma once
#include <stdexcept>
#include <memory>

namespace Engine {
	// Engine declarations
	class Base;
	class Logger;
	class Game;
	class GLFWContext;
	class Window;
	class InputHandler;
	class GameClock;
	class Scheduler;
	class EntityRegister;
	class EntityStressTest;
	class Renderer;
	class Shader;
	class VBO;
	class VAO;
	class EBO;
	class TickedSystem;
	class Mesh;
	class Texture;

	struct ActionMap;
	struct KeyStates;
	struct Vector3double;
	struct Vector3float;
	struct EntityID;
	struct ComponentPools;


	// Game declarations
	class MeshRegistry;
	class TextureRegistry;
	class MovementTicks;

	struct Movement;
	struct Position;
	struct Renderable;

	

	// Common types
	using i32 = std::int32_t;
	using i64 = std::int64_t;
	using ui32 = std::uint32_t;
	using f32 = float;
	using d64 = double;

	template <typename T> class ECSWrapper;
}