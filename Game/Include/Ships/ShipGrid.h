#pragma once
#include <Game.h>
#include <vector>

namespace Engine {
	struct GridCell {
		PartCategory partCategory = PartCategory::None;
		PartVariantID variant = INVALID_PART_VARIANT;
		bool isAnchor = true; // False for any cell that is not the anchor point of multi-cell parts
		i32 anchorX = -1, anchorY = -1;
		i32 sizeX = 1, sizeY = 1;
	};

	struct ShipGridDesc {
		BaseDesc base;
		i32 width;
		i32 height;
	};

	class ShipGrid : public Base {
	public: 
		explicit ShipGrid(const ShipGridDesc& desc);
		~ShipGrid();

		[[nodiscard]] i32 width() const noexcept { return m_width; };
		[[nodiscard]] i32 height() const noexcept { return m_height; };

		[[nodiscard]] const GridCell& at(i32 x, i32 y) const;
		[[nodiscard]] bool inBounds(i32 x, i32 y) const noexcept;

		[[nodiscard]] const GridCell& resolveAnchor(i32 x, i32 y) const;

		bool tryPlacePart(i32 x, i32 y, i32 sizeX, i32 sizeY, PartCategory category, PartVariantID variant);

		void forcePlacePart(i32 x, i32 y, i32 sizeX, i32 sizeY, PartCategory category, PartVariantID variant);

		void removePart(i32 x, i32 y);

	private:
		i32 index(i32 x, i32 y) const noexcept { return y * m_width + x; }

		i32 m_width;
		i32 m_height;

		std::vector<GridCell> m_cells;
	};
}