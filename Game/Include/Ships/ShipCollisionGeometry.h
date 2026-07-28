#pragma once
#include <Core/Common.h>
#include <Physics/Vector2float.h>
#include <Physics/Vector2double.h>
#include <vector>

namespace Engine {
	struct ShipGridData;

	struct GridEdge {
		Vector2float p0, p1;
	};

	struct ShipCollisionGeometry {
		std::vector<GridEdge> boundaryEdges;
		std::vector<Vector2float> hullSupportPoints; // Used in SAT method for mid-phase collision testing, convex hull vertices
	};

	struct EdgeHitResult {
		bool hit = false;
		Vector2double point{};
	};

	struct GridHitResult {
		bool hit = false;
		Vector2double point{};
		i32 cellX = -1, cellY = -1;
	};

	bool convexHullsOverlap(
		const std::vector<Vector2float>& hullA, const Vector2double& posA, f32 rotA,
		const std::vector<Vector2float>& hullB, const Vector2double& posB, f32 rotB
	);

	EdgeHitResult boundaryEdgesIntersect(
		const std::vector<GridEdge>& edgesA, const Vector2double& posA, f32 rotA,
		const std::vector<GridEdge>& edgesB, const Vector2double& posB, f32 rotB
	);

	GridHitResult raycastGrid(const ShipGridData& grid, const Vector2float& rayStart, const Vector2float& rayEnd);
}