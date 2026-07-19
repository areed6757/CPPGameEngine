#pragma once
#include <GameECS.h>

namespace Engine {
	struct MovementTicksDesc {
		BaseDesc base;
		GameECSWrapper& ecs;
	};

	struct EntityStressTestDesc {
		BaseDesc base;
		GameECSWrapper& ecs;
	};
}