// WeaponTest.h
#pragma once
#include <Game.h>

namespace Engine {
	struct WeaponTestDesc {
		BaseDesc base;
		GameECSWrapper& ecs;
	};

	// Spawns a stationary "ship" entity, a weapon mounted to it (WeaponMount
	// pointing at the ship, offset forward of its center), and an optional
	// stationary target with Health downrange -- lets you watch WeaponSystem
	// fire on cooldown and confirm the projectile hits and damages the
	// target, without bouncing off it or hitting the firer itself.
	class WeaponTest : public Base {
	public:
		explicit WeaponTest(const WeaponTestDesc& desc);
		~WeaponTest();

		void spawnFiringShip(d64 targetDistance, f32 cooldown, f32 projectileSpeed,
			f32 projectileRadius, f32 projectileDamage, f32 targetHealth);

	private:
		GameECSWrapper& m_ecs;
	};
}