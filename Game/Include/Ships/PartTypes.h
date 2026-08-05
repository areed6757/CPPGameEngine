#pragma once
#include <Core/Common.h>
#include <Physics/Vector2float.h>
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

		// Positive: this part provides system capacity (chassis/hull parts). Negative: this
		// part consumes it to be mounted (engines, hardpoint sockets, equipped weapons). Armor
		// deliberately defaults to 0 either way - it's pure protection, not chassis.
		f32 systemCapacityContribution{ 0.0f };
	};

	struct ArmorParams : PartBaseStats {
	};

	struct EngineParams : PartBaseStats { 
		f32 thrustForce;
		f32 powerDraw;
	};

	enum class WeaponRole : uint8_t {
		PointDefense,
		Flak,
		LightPrimary,
		StandardPrimary,
		RapidPrimary,
		HeavyPrimary,
		SiegePrimary,
		Torpedo,
		Count
	};

	struct WeaponParams : PartBaseStats {
		f32 damage, cooldown, projectileSpeed, projectileRadius;
		f32 projectileLifetime{ 3.0f };
		i32 barrelCount{ 1 };
		f32 barrelSpread{ 0.0f };
		f32 muzzleForwardOffset{ 0.0f };
		Vector2float anchorOffset{}; // local to the part's own unrotated space; mount attach point AND sprite rotation pivot
		f32 minRotation{ -PI }; // traverse arc, relative to the mount's rotation
		f32 maxRotation{ PI };
		f32 traverseSpeed{ 1000.0f }; // rad/s; large default == effectively instant snap for unconfigured weapons
		f32 accuracy{ 0.0f }; // cone half-angle (radians) of random spread at fire time; 0 == perfectly precise
		WeaponRole role{ WeaponRole::StandardPrimary };
	};

	struct HullParams : PartBaseStats {
	};

	struct HardpointParams : PartBaseStats {
		i32 sizeX;
		i32 sizeY;
	};
}