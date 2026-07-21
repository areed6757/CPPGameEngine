#pragma once
#include <Core/Core.h>
#include <Core/Base.h>
#include <Core/Logger.h>
#include <Physics/Vector2double.h>
#include <vector>

// Descriptions for dependency injection, constructors should only take these structs as args
namespace Engine {
	constexpr i32 INVALID_SENTINEL{ -1 };

	// Customization fields, to be moved later.
	constexpr Logger::LogLevel LOG_LEVEL{ Logger::LogLevel::Info };
	constexpr i32 WINDOW_WIDTH{ 1024 };
	constexpr i32 WINDOW_HEIGHT{ 1024 };
	constexpr const char* TITLE{ "Station Authority" };
	constexpr d64 TICK_RATE = { 1.0 / 60.0 }; // 1/60 tickRate is 60 ticks per second ***TODO : fix this to be 60 instead of 1/60
	constexpr const i32 MAX_ENTITIES = { 1'000'000 };
	constexpr const d64 GRID_CELL_SIZE_KM = 0.01; // Scale constant, .01 = 10m per chassis segment on ships
	constexpr const d64 gridBoundWidth = 2000.0;
	constexpr const d64 gridBoundHeight = 2000.0;

	// Core classes

	struct BaseDesc {
		Logger& logger;
	};

	struct GameDesc {
		Logger::LogLevel logLevel = LOG_LEVEL;

		i32 windowWidth = { WINDOW_WIDTH };
		i32 windowHeight = { WINDOW_HEIGHT };
		const char* title = { TITLE };
	};

	// Window and context creation

	struct GLFWDesc {
		BaseDesc base;
	};

	struct WindowDesc {
		BaseDesc base;

		// Default window fields set by instantiation in GameDesc
		i32 windowWidth = {};
		i32 windowHeight = {};
		const char* title = {};

		ActionMap& actionMap; // Pass-through for InputHandler of the Window
	};

	struct InputHandlerDesc {
		BaseDesc base;
		ActionMap& actionMap;
	};

	// Graphics

	struct TextureDesc {
		BaseDesc base;
		i32 widthImg = { 512 };
		i32 heightImg = { 512 };
		i32 colorChannels = { 4 }; // 3 = jpeg, 4 = png, currently concretely set in Texture instantiation to 4
		const char* imgAddr;
	};

	struct ShaderDesc {
		BaseDesc base;
		const char* vertexFile = "Shaders/default.vert";
		const char* fragmentFile = "Shaders/default.frag";
	};

	struct RendererDesc {
		BaseDesc base;
		Window& window;
		ShaderDesc shaderDesc; // Pass-through
		Camera& camera;
	};

	struct CameraDesc {
		BaseDesc base;
		Vector2double position{ 0.0, 0.0 };
		f32 zoom{ 10.0f };
	};

	struct DebugLineRendererDesc {
		BaseDesc base;
		ShaderDesc shaderDesc;
	};

	// Utilities

	struct GameClockDesc {
		BaseDesc base;
	};

	struct SchedulerDesc {
		BaseDesc base;
		GameClock& gameClock;
		d64 tickRate{ TICK_RATE };
	};


	// ECS

	struct EntityRegisterDesc {
		BaseDesc base;
		i32 maxEntities{ MAX_ENTITIES };
	};

	struct ComponentDesc {
		BaseDesc base;
		i32 maxEntities{ MAX_ENTITIES };
	};

	struct ECSWrapperDesc {
		BaseDesc base;
		EntityRegister& entityRegister;
		ComponentDesc& compDesc; // Pass-through for component pools owned by the ECSWrapper
	};

	// Physics

	struct QuadTreeDesc {
		BaseDesc base;
		Vector2double boundsMin{ -(gridBoundWidth /2), -(gridBoundHeight /2)};
		Vector2double boundsMax{ gridBoundWidth / 2, gridBoundHeight / 2 };
		i32 maxDepth{ 8 };
		i32 maxEntitiesPerNode{ 8 }; // Quadtree divides when exceeded up to maxDepth iterations
	};
}

