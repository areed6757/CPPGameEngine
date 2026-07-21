#pragma once

namespace Engine {
	struct Weapon {
		f32 cooldown;
		f32 timeSinceLastFire{};
		f32 projectileSpeed;
		f32 projectileRadius;
		f32 projectileDamage;
	};
}