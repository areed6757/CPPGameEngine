#pragma once

namespace Engine {
	struct Physics {
		f32 radius{ 0.05f }; // Broad-phase quadtree determination radius
		f32 mass{ 1.0f }; // <= 0.0 is treated as an immovable object, anchored to space
		f32 elasticity{ 0.2f }; // 0.0 is completely inelastic,  1.0 is completely elastic

		// Will act as a tag for entities to be checked for collisions
		// Holds things like ship moment-of-inertia, mass, etc.
		// Eventually directional propulsion will use this to calculate movement adjustments positionally
	};
}