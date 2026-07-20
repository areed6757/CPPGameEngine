#pragma once

namespace Engine {
	struct Physics {
		f32 radius{ 0.05f }; // Broad-phase quadtree determination radius
		// Will act as a tag for entities to be checked for collisions
		// Holds things like ship moment-of-inertia, mass, etc.
		// Eventually directional propulsion will use this to calculate movement adjustments positionally
	};
}