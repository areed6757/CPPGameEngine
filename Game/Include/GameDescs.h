#pragma once
#include <GameECS.h>

namespace Engine {
	struct EntityStressTestDesc {
		BaseDesc base;
		GameECSWrapper& ecs;
	};
}