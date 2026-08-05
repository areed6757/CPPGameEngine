#include <Ships/ShipGrid.h>
#include <Ships/ShipCollisionGeometry.h>
#include <algorithm>

namespace Engine {
	Engine::ShipGrid::ShipGrid(const ShipGridDesc& desc) : Base(desc.base),
		m_width(desc.width),
		m_height(desc.height),
		m_cells(static_cast<size_t>(desc.width) * desc.height)
	{
		//EngineLogInfo("ShipGrid created, ({}x{})", m_width, m_height);
	}

	ShipGrid::~ShipGrid()
	{

	}

	const GridCell& ShipGrid::at(i32 x, i32 y) const
	{
		static const GridCell empty{};
		if (!inBounds(x, y)) { return empty; }
		return m_cells[index(x, y)];
	}

	bool ShipGrid::inBounds(i32 x, i32 y) const noexcept
	{
		return x >= 0 && x < m_width && y >= 0 && y < m_height;
	}

	const GridCell& ShipGrid::resolveAnchor(i32 x, i32 y) const
	{
		const GridCell& cell = at(x, y);
		if (cell.partCategory == PartCategory::None || cell.isAnchor) { return cell; }
		return at(cell.anchorX, cell.anchorY);
	}

	bool ShipGrid::tryPlacePart(i32 x, i32 y, i32 sizeX, i32 sizeY, PartCategory category, PartVariantID variant, f32 rotation)
	{
		for (i32 dy = 0; dy < sizeY; dy++) {
			for (i32 dx = 0; dx < sizeX; dx++) {
				i32 cx = x + dx, cy = y + dy;
				if (!inBounds(cx, cy) || m_cells[index(cx, cy)].partCategory != PartCategory::None) { return false; }
			}
		}

		for (i32 dy = 0; dy < sizeY; dy++) {
			for (i32 dx = 0; dx < sizeX; dx++) {
				i32 cx = x + dx, cy = y + dy;
				GridCell& cell = m_cells[index(cx, cy)];
				cell.partCategory = category;
				cell.variant = variant;
				bool anchor = (dx == 0 && dy == 0);
				cell.isAnchor = anchor;
				cell.anchorX = anchor ? -1 : x;
				cell.anchorY = anchor ? -1 : y;
				cell.sizeX = sizeX;
				cell.sizeY = sizeY;
				cell.rotation = rotation;
			}
		}
		return true;
	}

	void ShipGrid::forcePlacePart(i32 x, i32 y, i32 sizeX, i32 sizeY, PartCategory category, PartVariantID variant, f32 rotation)
	{
		for (i32 dy = 0; dy < sizeY; dy++) {
			for (i32 dx = 0; dx < sizeX; dx++) {
				i32 cx = x + dx, cy = y + dy;
				if (!inBounds(cx, cy)) { continue; }

				GridCell& cell = m_cells[index(cx, cy)];
				cell.partCategory = category;
				cell.variant = variant;
				bool anchor = (dx == 0 && dy == 0);
				cell.isAnchor = anchor;
				cell.anchorX = anchor ? -1 : x;
				cell.anchorY = anchor ? -1 : y;
				cell.sizeX = sizeX;
				cell.sizeY = sizeY;
				cell.rotation = rotation;
			}
		}
	}

	void ShipGrid::removePart(i32 x, i32 y)
	{
		const GridCell& clicked = at(x, y);
		if (clicked.partCategory == PartCategory::None) { return; }

		i32 ax = clicked.isAnchor ? x : clicked.anchorX;
		i32 ay = clicked.isAnchor ? y : clicked.anchorY;

		const GridCell& anchor = m_cells[index(ax, ay)];
		i32 sizeX = anchor.sizeX;
		i32 sizeY = anchor.sizeY;

		for (i32 dy = 0; dy < sizeY; dy++) {
			for (i32 dx = 0; dx < sizeX; dx++) {
				i32 cx = ax + dx, cy = ay + dy;
				if (inBounds(cx, cy)) { m_cells[index(cx, cy)] = GridCell{}; }
			}
		}
	}

	ShipCollisionGeometry ShipGrid::buildCollisionGeometry() const
	{
		ShipCollisionGeometry geo;
		std::vector<Vector2float> occupiedCenters;

		f32 halfW = m_width * 0.5f;
		f32 halfH = m_height * 0.5f;
		f32 cellKm = static_cast<f32>(GRID_CELL_SIZE_KM);

		auto isOccupied = [&](i32 x, i32 y) -> bool {
			if (!inBounds(x, y)) { return false; }
			const GridCell& c = at(x, y);
			return c.partCategory != PartCategory::None;
			};

		for (i32 y = 0; y < m_height; y++) {
			for (i32 x = 0; x < m_width; x++) {
				if (!isOccupied(x, y)) { continue; }

				Vector2float center = gridAxesToLocal((x + 0.5f - halfW) * cellKm, (y + 0.5f - halfH) * cellKm);
				occupiedCenters.push_back(center);

				// A boundary cell is occupied with at least one empty/out-of-bounds
				// 4-neighbor. Emit that cell's edge on each empty-facing side.
				bool isBoundary =
					!isOccupied(x - 1, y) || !isOccupied(x + 1, y) ||
					!isOccupied(x, y - 1) || !isOccupied(x, y + 1);
				if (!isBoundary) { continue; }

				f32 x0 = (x - halfW) * cellKm, x1 = (x + 1 - halfW) * cellKm;
				f32 y0 = (y - halfH) * cellKm, y1 = (y + 1 - halfH) * cellKm;
				Vector2float c00 = gridAxesToLocal(x0, y0), c01 = gridAxesToLocal(x0, y1);
				Vector2float c10 = gridAxesToLocal(x1, y0), c11 = gridAxesToLocal(x1, y1);

				if (!isOccupied(x - 1, y)) { geo.boundaryEdges.push_back({ c00, c01 }); }
				if (!isOccupied(x + 1, y)) { geo.boundaryEdges.push_back({ c10, c11 }); }
				if (!isOccupied(x, y - 1)) { geo.boundaryEdges.push_back({ c00, c10 }); }
				if (!isOccupied(x, y + 1)) { geo.boundaryEdges.push_back({ c01, c11 }); }
			}
		}

		// Convex hull via Andrew's monotone chain over occupied cell centers
		std::sort(occupiedCenters.begin(), occupiedCenters.end(), [](const Vector2float& a, const Vector2float& b) {
			return a.x < b.x || (a.x == b.x && a.y < b.y);
			});
		auto cross = [](const Vector2float& o, const Vector2float& a, const Vector2float& b) {
			return (a.x - o.x) * (b.y - o.y) - (a.y - o.y) * (b.x - o.x);
			};
		std::vector<Vector2float> hull(2 * occupiedCenters.size());
		i32 k = 0;
		for (i32 i = 0; i < static_cast<i32>(occupiedCenters.size()); i++) {
			while (k >= 2 && cross(hull[k - 2], hull[k - 1], occupiedCenters[i]) <= 0) { k--; }
			hull[k++] = occupiedCenters[i];
		}
		i32 lower = k + 1;
		for (i32 i = static_cast<i32>(occupiedCenters.size()) - 2; i >= 0; i--) {
			while (k >= lower && cross(hull[k - 2], hull[k - 1], occupiedCenters[i]) <= 0) { k--; }
			hull[k++] = occupiedCenters[i];
		}
		hull.resize(k - 1);
		geo.hullSupportPoints = std::move(hull);

		return geo;
	}

	ShipGridData ShipGrid::toRuntimeData() const
	{
		ShipGridData data{ m_width, m_height };
		data.occupancy.reserve(m_cells.size());
		for (auto& cell : m_cells) { data.occupancy.push_back(cell.partCategory); }
		return data;
	}
}