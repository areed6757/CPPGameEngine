#include <Ships/ShipGrid.h>

namespace Engine {
	Engine::ShipGrid::ShipGrid(const ShipGridDesc& desc) : Base(desc.base),
		m_width(desc.width),
		m_height(desc.height),
		m_cells(static_cast<size_t>(desc.width) * desc.height)
	{
		EngineLogInfo("ShipGrid created, ({}x{})", m_width, m_height);
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

	bool ShipGrid::tryPlacePart(i32 x, i32 y, i32 sizeX, i32 sizeY, PartCategory category, PartVariantID variant)
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
			}
		}
		return true;
	}

	void ShipGrid::forcePlacePart(i32 x, i32 y, i32 sizeX, i32 sizeY, PartCategory category, PartVariantID variant)
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
}