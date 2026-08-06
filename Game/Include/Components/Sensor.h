#pragma once
#include <Core/Common.h>

namespace Engine {
	// detection-range multiplier applied to whatever a target's own SignalSignature implies, 1.0 baseline, dedicated sensor parts add to it
	struct Sensor {
		f32 power{ 1.0f };
	};
}
