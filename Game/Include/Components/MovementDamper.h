#pragma once

namespace Engine {
	struct MovementDamper {
		f32 linearDamping{ 2.5f }; // Percentage of linear velocity lost per tick when damping is active
		f32 angularDamping{ 2.5f }; // Same as linear but angular
		f32 linearStopThreshold{ 0.05f };
		f32 angularStopThreshold{ 0.05f };
	};
}