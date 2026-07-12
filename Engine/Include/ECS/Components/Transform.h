#pragma once
#include <Core/Common.h>
#include <Physics/Vector3double.h>

namespace Engine {
	struct Transform {
		Vector3double transform;
		f32 rotation;
	};
}