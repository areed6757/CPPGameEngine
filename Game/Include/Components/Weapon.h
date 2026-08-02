#pragma once

namespace Engine {
	struct Weapon {
		f32 cooldown;
		f32 timeSinceLastFire;
		f32 projectileSpeed;
		f32 projectileRadius;
		f32 projectileDamage;
		std::array<Vector2float, 4> barrelOffsets{}; // local to the mount's own position/facing, unused entries stay {0,0}
		i32 barrelCount{ 1 };
	};
}