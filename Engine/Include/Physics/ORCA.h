#pragma once
#include <Core/Common.h>
#include <Physics/Vector2float.h>
#include <vector>

namespace Engine {
	// permitted velocities v satisfy direction.cross(point - v) <= 0, left of the line through point along direction
	struct ORCALine {
		Vector2float point;
		Vector2float direction; // unit vector
	};

	// one reciprocal ORCA half-plane constraint against a neighbor, selfResponsibility (0..1) is how much of the avoidance this side takes, 1 means self dodges alone, 0 means self doesn't move, each side calls this independently with its own share
	[[nodiscard]] ORCALine computeORCALine(
		const Vector2float& selfVelocity,
		const Vector2float& relativePosition,
		const Vector2float& neighborVelocity,
		f32 combinedRadius,
		f32 timeHorizon,
		f32 timeStep,
		f32 selfResponsibility = 0.5f);

	// closest velocity to preferredVelocity satisfying every line, clamped to maxSpeed, falls back to least-penetrating if jointly infeasible
	[[nodiscard]] Vector2float solveORCA(const std::vector<ORCALine>& lines, const Vector2float& preferredVelocity, f32 maxSpeed);
}
