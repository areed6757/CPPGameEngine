#pragma once
#include <Core/Core.h>
#include <Core/Base.h>
#include <Core/Logger.h>
#include <ECS/Systems/TickedSystem.h>
#include <vector>

// Descriptions for dependency injection, constructors should only take these structs as args
namespace Engine {
	
	// Customization fields, to be moved later.
	constexpr Logger::LogLevel LOG_LEVEL{ Logger::LogLevel::Info };
	constexpr i32 WINDOW_WIDTH{ 600 };
	constexpr i32 WINDOW_HEIGHT{ 500 };
	constexpr const char* TITLE { "Station Authority" };
	constexpr d64 TICK_RATE = { 1.0 / 60.0 }; // 1/60 tickRate is 60 ticks per second ***TODO : fix this to be 60 instead of 1/60
	constexpr const i32 MAX_ENTITIES = { 1'000'000 };

	
	struct BaseDesc {
		Logger& logger;
	};

	struct GameDesc {
		Logger::LogLevel logLevel = LOG_LEVEL;

		i32 windowWidth = { WINDOW_WIDTH };
		i32 windowHeight = { WINDOW_HEIGHT };
		const char* title = { TITLE }; 
	};

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

	struct GameClockDesc {
		BaseDesc base;
	};

	struct SchedulerDesc {
		BaseDesc base;
		GameClock& gameClock;
		d64 tickRate{ TICK_RATE }; 
	};

	struct GraphicsTicksDesc {
		BaseDesc base;
	};

	struct MovementTicksDesc {
		BaseDesc base;
		ECSWrapper& ecs;
	};

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

	// Test Descriptions
	struct EntityStressTestDesc {
		BaseDesc base;
		ECSWrapper& ecsWrapper;
	};

}

