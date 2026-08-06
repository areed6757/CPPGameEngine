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

	// Home for ship-wide values baked once in ShipFactory::bake() that don't fit an existing "primitive" component (Physics, Thruster, Stability, ShipCollisionGeometry, ...)
	// derived combat/AI-facing scalars not grid/per-part shaped data, that's ShipGridData
	struct BakedShipStats {
		f32 idealFiringHeading{ 0.0f }; // ship-relative angle covered by the most overlapping weapon traverse arcs

		f32 primaryRange{ 0.0f }; // Weapon::maxRange of the ship's highest-volley-damage gun
		f32 idealRange{ 0.0f }; // each gun's Weapon::idealRange weighted by its effective DPS share

		bool isUtility{ false }; // stub - no onboard-system data model exists yet to compute this against
		bool isPointDefense{ false };

		f32 systemCapacityMax{ 0.0f }; // total provided by hull parts
		f32 systemCapacityUsed{ 0.0f }; // total consumed by engines, hardpoint sockets, and equipped weapons
		bool isOverSystemCapacity{ false };

		f32 totalSignalEmission{ 0.0f }; // sum of every part's signalEmissionValue, the ship's single unified targeting signal for now
		f32 totalSensorPower{ 0.0f }; // sum of every part's sensorPowerValue, added to a guaranteed 1.0 baseline for the ship's actual Sensor::power

		ShipClass shipClass{ ShipClass::Fighter };
	};
}
