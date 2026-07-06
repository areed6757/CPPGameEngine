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
	};

	struct GLFWDesc {
		BaseDesc base;
	};
}

