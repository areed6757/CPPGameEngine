#pragma once
#include <Core/Core.h>
#include <Core/Base.h>
#include <Core/Logger.h>
#include <ECS/Systems/TickedSystem.h>
#include <vector>

// Descriptions for dependency injection, constructors should only take these structs as args
namespace Engine {
	
	// Customization fields, to be moved later.
	i32 WINDOW_WIDTH{ 600 };
	i32 WINDOW_HEIGHT{ 500 };
	const char* TITLE { "Station Authority" };
	d64 TICK_RATE = { 1.0 / 60.0 }; // 1/60 tickRate is 60 ticks per second ***TODO : fix this to be 60 instead of 1/60
	const i32 MAX_ENTITIES = { 1000 };

	
	struct BaseDesc {
		Logger& logger;
	};

	struct GameDesc {
		Logger::LogLevel logLevel = Logger::LogLevel::Debug;

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

		ActionMap& actionMap; // Strictly pass-through for InputHandler of the Window
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

	struct EntityRegisterDesc {
		BaseDesc base;
		i32 maxEntities{ MAX_ENTITIES };
	};

	struct ComponentStorageDesc {
		BaseDesc base;
		i32 maxEntities{ MAX_ENTITIES };
	};
}

