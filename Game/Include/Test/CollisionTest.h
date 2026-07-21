#pragma once
#include <vector>
#include <Core/Base.h>
#include <Core/Common.h>
#include <GameECS.h>
#include <Physics/Vector2float.h>
#include <Physics/Vector2double.h>

namespace Engine {
	struct CollisionTestDesc {
		BaseDesc base;
		GameECSWrapper& ecs;
	};

	// Spawns pairs of entities launched directly at each other along a shared
	// axis so they collide head-on at a known point -- a controlled scenario
	// for validating CollisionSystem's impulse resolution (mass, elasticity),
	// as opposed to RenderGridTest/spawnProjectiles' scattered converge-on-
	// origin pattern.
	class CollisionTest : public Base {
	public:
		explicit CollisionTest(const CollisionTestDesc& desc);
		~CollisionTest();

		// Spawns pairCount pairs, each on its own parallel lane (offset along
		// the perpendicular axis by laneSpacing) so pairs don't interfere with
		// each other. Each pair starts separation apart and closes at speed.
		// massA/massB/elasticity apply identically to every pair -- call this
		// more than once with different values if you want mixed scenarios
		// (e.g. equal-mass elastic, heavy-vs-light, fully inelastic) in one run.
		void spawnHeadOnPairs(i32 pairCount, d64 separation, f32 speed,
			f32 physicsRadius, f32 massA, f32 massB, f32 elasticity, d64 laneSpacing);

	private:
		GameECSWrapper& m_ecs;
	};
}