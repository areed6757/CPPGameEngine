#pragma once

namespace Engine {
	struct MovementDamper {
		f32 linearDamping; // Percentage of linear velocity lost per tick when damping is active
		f32 angularDamping; // Same as linear but angular
	};
}