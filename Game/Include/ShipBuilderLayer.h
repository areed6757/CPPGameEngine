#pragma once
#include <Core/Base.h>
#include <Ships/ShipBuilder.h>
#include <Ships/ShipFactory.h>
#include <Ships/PartRegistry.h>
#include <Ships/PartVariant.h>
#include <GameECS.h>
#include <string>
#include <vector>

namespace Engine {
	struct ShipBuilderLayerDesc {
		BaseDesc base;
		ShipBuilder& builder;
		PartRegistry& partRegistry;
		ShipFactory& shipFactory;
		GameECSWrapper& ecs;
	};

	class ShipBuilderLayer : public Base {
	public:
		explicit ShipBuilderLayer(const ShipBuilderLayerDesc& desc);

		void draw();

	private:
		void drawPalette();
		void drawGrid();
		void drawHardpointLoadout();
		void drawLoadSection();
		void drawStatsPanel();
		void save();
		void refreshSavedShipList();

		void spawnShip(const ShipGrid& grid, const HardpointLoadout& loadout);

		ShipBuilder& m_builder;
		PartRegistry& m_partRegistry;
		ShipFactory& m_shipFactory;
		GameECSWrapper& m_ecs;

		PartVariantID m_selectedVariant{ INVALID_PART_VARIANT };
		std::string m_shipName{ "New Ship" };
		std::string m_saveStatus;

		std::vector<std::string> m_savedShipNames;
		i32 m_selectedSavedShip{ -1 };
		i32 m_spawnCardinalIndex{ 0 };
	};
}
