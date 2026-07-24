#pragma once
#include <Core/Base.h>
#include <Core/Common.h>
#include <GameECS.h>

namespace Engine {
	struct ThreadingStressTestDesc {
		BaseDesc base;
		GameECSWrapper& ecs;
	};

	// Spawns a large grid of Position+Movement-only entities (deliberately no
	// Physics/Renderable -- isolates MovementTicks's integration cost from
	// collision broad-phase and draw-call overhead) with varied per-entity
	// acceleration, so MovementTicks has genuinely divergent, real work to do
	// per entity rather than uniform trivially-optimizable motion.
	class ThreadingStressTest : public Base {
	public:
		explicit ThreadingStressTest(const ThreadingStressTestDesc& desc);
		~ThreadingStressTest();

		void spawnGrid(i32 count, d64 spacing);

	private:
		GameECSWrapper& m_ecs;
	};
}