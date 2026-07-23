#pragma once
#include <Core/Base.h>
#include <Core/Common.h>
#include <GameECS.h>

namespace Engine {
	struct CoreSystemsTestDesc {
		BaseDesc base;
		GameECSWrapper& ecs;
	};

	// Combines CollisionTest/ThrusterTest/DamageTest's individual scenarios
	// into one spawn call, each on its own row (offset along Y) so the three
	// don't interfere with each other spatially: a thrust-accelerating row,
	// a head-on mass/elasticity collision pair, and a damage-on-hit pair.
	// One scene to eyeball movement, collision, and damage together.
	class CoreSystemsTest : public Base {
	public:
		explicit CoreSystemsTest(const CoreSystemsTestDesc& desc);
		~CoreSystemsTest();

		void spawnAll();

	private:
		GameECSWrapper& m_ecs;
	};
}