#pragma once
#include <Core/Core.h>
#include <Core/Base.h>
#include <Core/Logger.h>

// Descriptions for dependency injection, constructors should only take these structs as args
namespace Engine {
	struct BaseDesc {
		Logger& logger;
	};

	struct GameDesc {
		Logger::LogLevel logLevel = Logger::LogLevel::Error;

		i32 windowWidth = { 1280 };
		i32 windowHeight = { 1020 };
		const char* title = { "Station Authority" };
	};

	struct GLFWDesc {
		BaseDesc base;
	};

	struct WindowDesc {
		BaseDesc base;
		// Default window fields
		i32 windowWidth = { 1280 };
		i32 windowHeight = { 1020 };
		const char* title = { "Station Authority" };
	};
}

