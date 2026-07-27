#pragma once

namespace Engine {
	struct DamagePayload {
		f32 amount;
		EntityID source; // Used to check for friendly/self fire
	};
}