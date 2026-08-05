#pragma once
#include <Core/Common.h>
#include <Ships/PartTypes.h>
#include <Ships/PartVariant.h>
#include <vector>

namespace Engine {
	struct ShipCollisionGeometry;

	struct ShipGridData {
		i32 width, height;
		std::vector<PartCategory> occupancy; // one entry per cell
	};

	struct GridCell {
		PartCategory partCategory = PartCategory::None;
		PartVariantID variant = INVALID_PART_VARIANT;
		bool isAnchor = true; // False for any cell that is not the anchor point of multi-cell parts
		i32 anchorX = -1, anchorY = -1;
		i32 sizeX = 1, sizeY = 1;
		f32 rotation = 0.0f; // ship-relative facing; only meaningful for Hardpoint cells, baked into Mount::rotation
	};

	struct ShipGridDesc {
		BaseDesc base;
		i32 width;
		i32 height;
	};

	// grid row 0 is the ship's forward edge, rotates grid col/row axes into local ship space where forward is +x
	inline Vector2float gridAxesToLocal(f32 gridX, f32 gridY) { return Vector2float{ -gridY, gridX }; }
	inline Vector2float localAxesToGrid(f32 localX, f32 localY) { return Vector2float{ localY, -localX }; }

	class ShipGrid : public Base {
	public: 
		explicit ShipGrid(const ShipGridDesc& desc);
		~ShipGrid();

		[[nodiscard]] i32 width() const noexcept { return m_width; };
		[[nodiscard]] i32 height() const noexcept { return m_height; };

		[[nodiscard]] const GridCell& at(i32 x, i32 y) const;
		[[nodiscard]] bool inBounds(i32 x, i32 y) const noexcept;

		[[nodiscard]] const GridCell& resolveAnchor(i32 x, i32 y) const;

		[[nodiscard]] ShipCollisionGeometry buildCollisionGeometry() const;

		[[nodiscard]] ShipGridData toRuntimeData() const;

		bool tryPlacePart(i32 x, i32 y, i32 sizeX, i32 sizeY, PartCategory category, PartVariantID variant, f32 rotation = 0.0f);

		void forcePlacePart(i32 x, i32 y, i32 sizeX, i32 sizeY, PartCategory category, PartVariantID variant, f32 rotation = 0.0f);

		void removePart(i32 x, i32 y);

	private:
		i32 index(i32 x, i32 y) const noexcept { return y * m_width + x; }

		i32 m_width;
		i32 m_height;

		std::vector<GridCell> m_cells;
	};
}