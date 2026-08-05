#pragma once
#include <Ships/PartTypes.h>

namespace Engine {
	struct Weapon {
		f32 cooldown;
		f32 timeSinceLastFire;
		f32 projectileSpeed;
		f32 projectileRadius;
		f32 projectileDamage;
		f32 projectileLifetime;
		std::array<Vector2float, 4> barrelOffsets{}; // local to the mount's own position/facing, unused entries stay {0,0}
		i32 barrelCount{ 1 };

		f32 aimRotation{ 0.0f }; // ship-relative current aim angle
		f32 minRotationAbs{ 0.0f }; // baked: mount.rotation + WeaponParams::minRotation
		f32 maxRotationAbs{ 0.0f }; // baked: mount.rotation + WeaponParams::maxRotation
		f32 restRotationAbs{ 0.0f }; // baked midpoint, held when no target
		f32 traverseSpeed{ 1000.0f }; // rad/s, max angular speed of aimRotation tracking
		f32 accuracy{ 0.0f }; // cone half-angle (radians) of random spread applied at fire time

		f32 maxRange{ 0.0f };
		f32 idealRange{ 0.0f };
		WeaponRole role{ WeaponRole::StandardPrimary };

		bool targetInRange{ false };
	};
}