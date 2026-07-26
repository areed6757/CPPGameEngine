#pragma once
#include <stdexcept>
#include <memory>

namespace Engine {
	// Engine declarations
	class Base;
	class Logger;
	class Application;
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
	class Camera;
	class CameraController;
	class QuadTree;
	class ThreadPool;
	class JobController;
	class Layer;
	class LayerStack;

	template <typename TPools> class ECSWrapper;

	struct ActionMap;
	struct KeyStates;
	struct Vector3double;
	struct Vector3float;
	struct EntityID;
	struct ComponentPools;
	struct Job;

	// Common types
	using i32 = std::int32_t;
	using i64 = std::int64_t;
	using ui32 = std::uint32_t;
	using f32 = float;
	using d64 = double;
}