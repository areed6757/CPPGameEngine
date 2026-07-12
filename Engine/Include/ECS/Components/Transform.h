#pragma once
#include <Core/Common.h>
#include <Physics/Vector3double.h>

namespace Engine {
	struct Transform {
		Vector3double transform{0.0, 0.0, 0.0};
		f32 rotation{0.0};
	};
}