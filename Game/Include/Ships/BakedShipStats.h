#pragma once
#include <Core/Common.h>
#include <cstdint>

namespace Engine {
	enum class ShipClass : uint8_t {
		Fighter,
		Frigate,
		Destroyer,
		Cruiser,
		Battleship
	};

	// Home for ship-wide values baked once in ShipFactory::bake() that don't fit an existing
	// "primitive" component (Physics, Thruster, Stability, ShipCollisionGeometry, ...) - e.g.
	// derived combat/AI-facing scalars. Not grid/per-part shaped data; that's ShipGridData.
	struct BakedShipStats {
		f32 idealFiringHeading{ 0.0f }; // ship-relative angle covered by the most overlapping weapon traverse arcs

		f32 primaryRange{ 0.0f }; // Weapon::maxRange of the ship's highest-volley-damage gun
		f32 idealRange{ 0.0f }; // each gun's Weapon::idealRange weighted by its effective DPS share

		bool isUtility{ false }; // stub - no onboard-system data model exists yet to compute this against
		bool isPointDefense{ false };

		ShipClass shipClass{ ShipClass::Fighter };
	};
}
