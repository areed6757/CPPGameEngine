#pragma once
#include <vector>
#include <Core/Base.h>
#include <Core/Common.h>
#include <GameECS.h>

namespace Engine {
	struct ThrusterTestDesc {
		BaseDesc base;
		GameECSWrapper& ecs;
	};

	// Spawns a row of entities, evenly spaced along Y, each with a different
	// Thruster::maxAccel (linear step from minAccel to maxAccel across the
	// row), all facing the same direction and starting at rest -- lets you
	// visually compare acceleration curves side by side since they share a
	// start line and diverge purely from differing accel.
	class ThrusterTest : public Base {
	public:
		explicit ThrusterTest(const ThrusterTestDesc& desc);
		~ThrusterTest();

		void spawnAccelRow(i32 count, d64 laneSpacing, f32 minAccel, f32 maxAccel,
			f32 physicsRadius, f32 facingRotation);

	private:
		GameECSWrapper& m_ecs;
	};
}