#pragma once
#include <Core/Common.h>
#include <Physics/Vector2double.h>

namespace Engine {
	struct Position {
		Vector2double transform{0.0, 0.0};
		f32 rotation{0.0};

		// snapshotted once per fixed tick by TransformHistorySystem, consumed only by rendering to smooth over the fixed/frame rate mismatch
		Vector2double prevTransform{0.0, 0.0};
		f32 prevRotation{0.0};
		bool hasHistory{false};
	};

	struct InterpolatedPose {
		Vector2double transform;
		f32 rotation;
	};

	// alpha in [0,1], how far the sim is between the last fixed tick and the next, falls back to the exact pose until a tick has snapshotted history
	inline InterpolatedPose interpolatePosition(const Position& pos, f32 alpha) {
		if (!pos.hasHistory) { return { pos.transform, pos.rotation }; }

		Vector2double transform = pos.prevTransform + (pos.transform - pos.prevTransform) * static_cast<d64>(alpha);

		f32 delta = pos.rotation - pos.prevRotation;
		while (delta > PI) { delta -= 2.0f * PI; }
		while (delta < -PI) { delta += 2.0f * PI; }

		return { transform, pos.prevRotation + delta * alpha };
	}
}