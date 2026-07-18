#pragma once
#include <Core/Common.h>
#include <Physics/Vector2float.h>

namespace Engine {
	struct Movement {
		Vector2float linearVelocity;
		f32 angularVelocity;

		Vector2float linearAcceleration;
		f32 angularAcceleration;
	};
}