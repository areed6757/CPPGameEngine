#pragma once

namespace Engine {
	struct Mount {
		EntityID owner;
		Vector2float offset;
		f32 rotation{ 0.0f }; // hardpoint's base/preset facing, ship-relative
	};
}