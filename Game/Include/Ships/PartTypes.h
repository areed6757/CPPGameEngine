#pragma once
#include <Core/Common.h>
#include <cstdint>

namespace Engine {
	enum class PartCategory : uint8_t {
		None = 0,
		Hull,
		Armor,
		Engine,
		Weapon, // Attaches to hardpoint parts
		Hardpoint,
		Count
	};

	struct PartBaseStats {
		f32 mass;
		f32 health;
		f32 stabilityContribution;

		f32 kineticResistance;
	};

	struct ArmorParams : PartBaseStats {
	};

	struct EngineParams : PartBaseStats { 
		f32 thrustForce;
		f32 powerDraw;
	};

	struct WeaponParams : PartBaseStats {
		f32 damage;
		f32 cooldown;
		f32 projectileSpeed;
	};

	struct HullParams : PartBaseStats {
	};

	struct HardpointParams : PartBaseStats {
		i32 sizeX;
		i32 sizeY;
	};
}