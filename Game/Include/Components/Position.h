#pragma once
#include <Core/Common.h>
#include <Physics/Vector2double.h>

namespace Engine {
	struct Position {
		Vector2double transform{0.0, 0.0};
		f32 rotation{0.0};
	};
}