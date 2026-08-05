#include <Ships/ShipCollisionGeometry.h>
#include <Ships/ShipGrid.h>
#include <cmath>
#include <limits>

namespace Engine {
	namespace {
		std::vector<Vector2double> transformHull(const std::vector<Vector2float>& local, const Vector2double& pos, f32 rot)
		{
			std::vector<Vector2double> world;
			world.reserve(local.size());
			f32 c = std::cos(rot), s = std::sin(rot);
			for (auto& p : local) {
				world.push_back(Vector2double{
					pos.x + static_cast<d64>(p.x * c - p.y * s),
					pos.y + static_cast<d64>(p.x * s + p.y * c)
					});
			}
			return world;
		}

		// Projects every point of a hull onto axis, returns [min, max]
		void projectHull(const std::vector<Vector2double>& hull, const Vector2double& axis, d64& outMin, d64& outMax)
		{
			outMin = std::numeric_limits<d64>::max();
			outMax = std::numeric_limits<d64>::lowest();
			for (auto& p : hull) {
				d64 proj = p.x * axis.x + p.y * axis.y;
				outMin = std::min(outMin, proj);
				outMax = std::max(outMax, proj);
			}
		}

		// Tests every edge normal of one hull as a candidate separating axis against both hulls
		bool testAxesOf(const std::vector<Vector2double>& edgeSource, const std::vector<Vector2double>& hullA, const std::vector<Vector2double>& hullB)
		{
			size_t n = edgeSource.size();
			for (size_t i = 0; i < n; i++) {
				Vector2double p0 = edgeSource[i];
				Vector2double p1 = edgeSource[(i + 1) % n];
				Vector2double edge{ p1.x - p0.x, p1.y - p0.y };
				Vector2double axis{ -edge.y, edge.x };

				d64 len = std::sqrt(axis.x * axis.x + axis.y * axis.y);
				if (len < 1e-9) { continue; }
				axis.x /= len; axis.y /= len;

				d64 minA, maxA, minB, maxB;
				projectHull(hullA, axis, minA, maxA);
				projectHull(hullB, axis, minB, maxB);

				if (maxA < minB || maxB < minA) { return false; } // separating axis found, no collision
			}
			return true;
		}
	}

	bool convexHullsOverlap(const std::vector<Vector2float>& hullA, const Vector2double& posA, f32 rotA,
		const std::vector<Vector2float>& hullB, const Vector2double& posB, f32 rotB)
	{
		if (hullA.empty() || hullB.empty()) { return false; }

		auto worldA = transformHull(hullA, posA, rotA);
		auto worldB = transformHull(hullB, posB, rotB);

		if (!testAxesOf(worldA, worldA, worldB)) { return false; }
		if (!testAxesOf(worldB, worldA, worldB)) { return false; }

		return true;
	}

	Vector2double transformPoint(const Vector2float& local, const Vector2double& pos, f32 c, f32 s)
	{
		return Vector2double{
			pos.x + static_cast<d64>(local.x * c - local.y * s),
			pos.y + static_cast<d64>(local.x * s + local.y * c)
		};
	}

	// Returns false for parallel segments, edge/edge grazes are treated as a miss
	bool segmentIntersect(const Vector2double& p1, const Vector2double& p2,
		const Vector2double& p3, const Vector2double& p4, Vector2double& outPoint)
	{
		d64 d1x = p2.x - p1.x, d1y = p2.y - p1.y;
		d64 d2x = p4.x - p3.x, d2y = p4.y - p3.y;
		d64 denom = d1x * d2y - d1y * d2x;
		if (std::abs(denom) < 1e-12) { return false; }

		d64 t = ((p3.x - p1.x) * d2y - (p3.y - p1.y) * d2x) / denom;
		d64 u = ((p3.x - p1.x) * d1y - (p3.y - p1.y) * d1x) / denom;
		if (t < 0.0 || t > 1.0 || u < 0.0 || u > 1.0) { return false; }

		outPoint = Vector2double{ p1.x + t * d1x, p1.y + t * d1y };
		return true;
	}

	EdgeHitResult boundaryEdgesIntersect(const std::vector<GridEdge>& edgesA, const Vector2double& posA, f32 rotA,
		const std::vector<GridEdge>& edgesB, const Vector2double& posB, f32 rotB)
	{
		EdgeHitResult result;
		if (edgesA.empty() || edgesB.empty()) { return result; }

		f32 cA = std::cos(rotA), sA = std::sin(rotA);
		f32 cB = std::cos(rotB), sB = std::sin(rotB);

		for (auto& eb : edgesB) {
			Vector2double b0 = transformPoint(eb.p0, posB, cB, sB);
			Vector2double b1 = transformPoint(eb.p1, posB, cB, sB);

			for (auto& ea : edgesA) {
				Vector2double a0 = transformPoint(ea.p0, posA, cA, sA);
				Vector2double a1 = transformPoint(ea.p1, posA, cA, sA);

				Vector2double hitPoint;
				if (segmentIntersect(a0, a1, b0, b1, hitPoint)) {
					result.hit = true;
					result.point = hitPoint;
					return result;
				}
			}
		}
		return result;
	}

	GridHitResult raycastGrid(const ShipGridData& grid, const Vector2float& rayStart, const Vector2float& rayEnd)
	{
		GridHitResult result;
		f32 cellKm = static_cast<f32>(GRID_CELL_SIZE_KM);
		f32 halfW = grid.width * 0.5f, halfH = grid.height * 0.5f;

		auto toCell = [&](const Vector2float& p) -> Vector2float {
			Vector2float g = localAxesToGrid(p.x, p.y);
			return Vector2float{ g.x / cellKm + halfW, g.y / cellKm + halfH };
			};
		Vector2float start = toCell(rayStart);
		Vector2float end = toCell(rayEnd);

		Vector2float delta{ end.x - start.x, end.y - start.y };
		f32 dist = std::sqrt(delta.x * delta.x + delta.y * delta.y);
		if (dist < 1e-6f) { return result; }

		i32 steps = static_cast<i32>(std::ceil(dist)) + 1;
		f32 stepX = delta.x / steps, stepY = delta.y / steps;

		f32 cx = start.x, cy = start.y;
		for (i32 i = 0; i <= steps; i++) {
			i32 gx = static_cast<i32>(std::floor(cx));
			i32 gy = static_cast<i32>(std::floor(cy));

			bool inBounds = gx >= 0 && gx < grid.width && gy >= 0 && gy < grid.height;
			if (inBounds && grid.occupancy[gy * grid.width + gx] != PartCategory::None) {
				result.hit = true;
				result.cellX = gx;
				result.cellY = gy;
				Vector2float local = gridAxesToLocal((cx - halfW) * cellKm, (cy - halfH) * cellKm);
				result.point = Vector2double{ local.x, local.y };
				return result;
			}
			cx += stepX; cy += stepY;
		}
		return result;
	}
}