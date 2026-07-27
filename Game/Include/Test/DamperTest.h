#pragma once
#include <Game.h>

namespace Engine {
	struct DamperTestDesc {
		BaseDesc base;
		GameECSWrapper& ecs;
	};

	// Spawns entities to compare damper behavior: one coasting (no Thruster
	// at all, damper should always apply), one with throttle at zero
	// (Thruster present but idle, damper should apply), and one actively
	// thrusting (damper should NOT apply, per the "only damp when not
	// deliberately accelerating" design). All start with the same initial
	// velocity so the divergence is directly comparable.
	class DamperTest : public Base {
	public:
		explicit DamperTest(const DamperTestDesc& desc);
		~DamperTest();

		void spawnComparisonRow(d64 laneSpacing, f32 initialSpeed,
			f32 linearDamping, f32 angularDamping);

	private:
		GameECSWrapper& m_ecs;
	};
}