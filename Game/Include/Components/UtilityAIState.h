#pragma once
#include <Core/Common.h>

namespace Engine {
	// Stub - nothing reads or writes this yet.
	struct UtilityAIState {
		bool isFlagship{ false };

		f32 currentAggression{ 0.0f };
		f32 maxAggression{ 0.0f };
		f32 currentFear{ 0.0f };
		f32 maxFear{ 0.0f };
		f32 currentStrategic{ 0.0f };
		f32 maxStrategic{ 0.0f };
	};
}
