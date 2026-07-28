#pragma once
#include <Core/Core.h>
#include <Core/Base.h>
#include <Core/Logger.h>
#include <Physics/Vector2double.h>
#include <vector>
#include <thread>

namespace Engine {
	constexpr i32 INVALID_SENTINEL{ -1 };

	constexpr Logger::LogLevel LOG_LEVEL{ Logger::LogLevel::Info };
	constexpr i32 WINDOW_WIDTH{ 1024 };
	constexpr i32 WINDOW_HEIGHT{ 1024 };
	constexpr const char* TITLE{ "Stacked Engine" };
	constexpr d64 TICK_RATE = { 1.0 / 60.0 }; // 1/60 tickRate is 60 ticks per second ***TODO : fix this to be 60 instead of 1/60
	constexpr const i32 MAX_ENTITIES = { 100'000 };
	constexpr const d64 GRID_CELL_SIZE_KM = 0.01; // Scale constant, .01 = 10m per chassis segment on ships
	constexpr const d64 GRID_BOUND_WIDTH = 500.0;
	constexpr const d64 GRID_BOUND_HEIGHT = 500.0;

	struct BaseDesc {
		Logger& logger;
	};
}

