#pragma once

namespace Engine {
	struct Thruster {
		f32 maxAccel;
		f32 throttle{ 1.0f }; // 0.0 -> 1.0 scaling throttle percentage
	};
}