#pragma once
#include <Core/Common.h>

namespace Engine {
	// Home for ship-wide values baked once in ShipFactory::bake() that don't fit an existing
	// "primitive" component (Physics, Thruster, Stability, ShipCollisionGeometry, ...) - e.g.
	// derived combat/AI-facing scalars. Not grid/per-part shaped data; that's ShipGridData.
	struct BakedShipStats {
		f32 idealFiringHeading{ 0.0f }; // ship-relative angle covered by the most overlapping weapon traverse arcs
	};
}
