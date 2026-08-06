#pragma once

namespace Engine {
	struct Separation {
		f32 margin{ 0.5f };
		f32 timeHorizon{ 3.0f }; // ORCA lookahead window in seconds, how far ahead to anticipate and steer around same-team ships
	};
}