#pragma once

namespace Engine {
	struct Thruster {
		f32 maxAccel;
		f32 maxVelocity{ 0.0f }; // km/s hard speed cap, MovementTicks clamps to this after integrating, <= 0 means uncapped
		f32 turnRate{ 0.5f }; // rad/s hull turn cap, AISystem slews facing toward its target at this rate instead of snapping
		f32 throttle{ 1.0f }; // 0.0 -> 1.0 scaling throttle percentage
	};
}