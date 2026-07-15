#pragma once
#include <Core/Common.h>

namespace Engine {
	struct Movement {
		f32 linearVelocity;
		f32 angularVelocity;

		f32 linearAcceleration;
		f32 angularAcceleration;
	};
}