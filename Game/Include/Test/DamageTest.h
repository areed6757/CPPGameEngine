#pragma once
#include <Core/Base.h>
#include <Core/Common.h>
#include <GameECS.h>

namespace Engine {
	struct DamageTestDesc {
		BaseDesc base;
		GameECSWrapper& ecs;
	};

	// Spawns one stationary target (Health) and one projectile (DamagePayload)
	// launched directly at it, for isolated verification of DamageSystem:
	// confirms damage applies, target destroys at zero health, projectile
	// always destroys on hit, and no impulse/bounce occurs on the pair.
	class DamageTest : public Base {
	public:
		explicit DamageTest(const DamageTestDesc& desc);
		~DamageTest();

		void spawnTestHit(d64 distance, f32 speed, f32 physicsRadius,
			f32 targetHealth, f32 payloadDamage);

	private:
		GameECSWrapper& m_ecs;
	};
}