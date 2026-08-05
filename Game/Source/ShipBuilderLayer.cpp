#include <ShipBuilderLayer.h>
#include <Serialization/ShipBlueprintSerializer.h>
#include <Components/Faction.h>
#include <Components/Separation.h>
#include <Components/AIController.h>
#include <imgui.h>
#include <string>
#include <filesystem>
#include <algorithm>
#include <cmath>

namespace Engine {
	namespace {
		const char* categoryShortLabel(PartCategory category) {
			switch (category) {
			case PartCategory::Hull: return "H";
			case PartCategory::Armor: return "A";
			case PartCategory::Engine: return "E";
			case PartCategory::Hardpoint: return "P";
			default: return "?";
			}
		}

		const char* categoryName(PartCategory category) {
			switch (category) {
			case PartCategory::Hull: return "Hull";
			case PartCategory::Armor: return "Armor";
			case PartCategory::Engine: return "Engine";
			case PartCategory::Hardpoint: return "Hardpoint";
			default: return "Unknown";
			}
		}

		ImVec4 categoryColor(PartCategory category) {
			switch (category) {
			case PartCategory::Hull: return ImVec4(0.45f, 0.45f, 0.50f, 1.0f);
			case PartCategory::Armor: return ImVec4(0.55f, 0.40f, 0.20f, 1.0f);
			case PartCategory::Engine: return ImVec4(0.20f, 0.45f, 0.60f, 1.0f);
			case PartCategory::Hardpoint: return ImVec4(0.60f, 0.20f, 0.20f, 1.0f);
			default: return ImVec4(0.30f, 0.30f, 0.30f, 1.0f);
			}
		}
	}

	// beyond the ~57km half-diagonal of the spawnTieredFleetBattle formation
	constexpr f32 SHIP_BUILDER_SPAWN_RADIUS_KM = 84.0f;

	// NE, SE, SW, NW
	constexpr f32 SHIP_BUILDER_CARDINAL_ANGLES[4] = { PI * 0.25f, -PI * 0.25f, PI * 1.25f, PI * 0.75f };
	constexpr i32 SHIP_BUILDER_CARDINAL_TEAM_IDS[4] = { 1, 2, 3, 4 };

	ShipBuilderLayer::ShipBuilderLayer(const ShipBuilderLayerDesc& desc) : Base(desc.base),
		m_builder(desc.builder),
		m_partRegistry(desc.partRegistry),
		m_shipFactory(desc.shipFactory),
		m_ecs(desc.ecs)
	{
		m_shipName.resize(64, '\0');
		refreshSavedShipList();
	}

	void ShipBuilderLayer::drawPalette()
	{
		ImGui::Text("Parts");
		ImGui::BeginChild("ShipBuilderPalette", ImVec2(0.0f, 120.0f), ImGuiChildFlags_Borders);
		for (i32 i = 0; i < m_partRegistry.size(); i++) {
			const PartVariant& variant = m_partRegistry.get(i);
			if (variant.category == PartCategory::Weapon) { continue; }

			bool selected = (m_selectedVariant == i);
			std::string label = variant.name + " (" + categoryName(variant.category) + ")";
			if (ImGui::Selectable(label.c_str(), selected)) {
				m_selectedVariant = i;
			}
		}
		ImGui::EndChild();
	}

	void ShipBuilderLayer::drawGrid()
	{
		ImGui::Text("Grid -- click an empty cell to place the selected part, click a filled cell to remove it");

		const ShipGrid& grid = m_builder.grid();
		constexpr f32 CELL_SIZE = 24.0f;

		for (i32 y = 0; y < grid.height(); y++) {
			for (i32 x = 0; x < grid.width(); x++) {
				if (x > 0) { ImGui::SameLine(0.0f, 2.0f); }

				const GridCell& cell = grid.at(x, y);
				bool occupied = cell.partCategory != PartCategory::None;

				ImGui::PushID(y * grid.width() + x);
				if (occupied) { ImGui::PushStyleColor(ImGuiCol_Button, categoryColor(cell.partCategory)); }

				if (ImGui::Button(occupied ? categoryShortLabel(cell.partCategory) : "##empty", ImVec2(CELL_SIZE, CELL_SIZE))) {
					if (occupied) {
						m_builder.removePart(x, y);
					}
					else if (m_selectedVariant != INVALID_PART_VARIANT) {
						m_builder.placePart(x, y, m_selectedVariant);
					}
				}

				if (occupied) { ImGui::PopStyleColor(); }
				ImGui::PopID();
			}
		}
	}

	void ShipBuilderLayer::drawHardpointLoadout()
	{
		ImGui::Text("Hardpoint Loadout");

		const ShipGrid& grid = m_builder.grid();
		bool anyHardpoints = false;

		for (i32 y = 0; y < grid.height(); y++) {
			for (i32 x = 0; x < grid.width(); x++) {
				const GridCell& cell = grid.at(x, y);
				if (cell.partCategory != PartCategory::Hardpoint || !cell.isAnchor) { continue; }

				anyHardpoints = true;
				PartVariantID current = m_builder.weaponAt(x, y);
				std::string previewLabel = (current != INVALID_PART_VARIANT) ? m_partRegistry.get(current).name : "None";

				ImGui::PushID(y * grid.width() + x);
				std::string comboLabel = "Hardpoint (" + std::to_string(x) + ", " + std::to_string(y) + ")";
				if (ImGui::BeginCombo(comboLabel.c_str(), previewLabel.c_str())) {
					if (ImGui::Selectable("None", current == INVALID_PART_VARIANT)) {
						m_builder.clearWeapon(x, y);
					}
					for (i32 i = 0; i < m_partRegistry.size(); i++) {
						const PartVariant& variant = m_partRegistry.get(i);
						if (variant.category != PartCategory::Weapon) { continue; }

						if (ImGui::Selectable(variant.name.c_str(), current == i)) {
							m_builder.assignWeapon(x, y, i);
						}
					}
					ImGui::EndCombo();
				}
				ImGui::PopID();
			}
		}

		if (!anyHardpoints) { ImGui::TextDisabled("(no hardpoints placed yet)"); }
	}

	void ShipBuilderLayer::save()
	{
		std::string trimmedName = m_shipName.substr(0, m_shipName.find('\0'));
		if (trimmedName.empty()) {
			m_saveStatus = "Save FAILED: ship needs a name";
			return;
		}

		std::filesystem::path outPath = std::filesystem::path(SOURCE_ROOT) / "Assets" / "Ships" / (trimmedName + ".json");
		std::filesystem::create_directories(outPath.parent_path());

		if (!saveShipBlueprint(trimmedName, m_builder.grid(), m_partRegistry, m_builder.hardpointLoadout(), outPath)) {
			m_saveStatus = "Save FAILED: " + outPath.string();
			return;
		}

		refreshSavedShipList();
		m_saveStatus = "Saved: " + outPath.string();
	}

	void ShipBuilderLayer::spawnShip(const ShipGrid& grid, const HardpointLoadout& loadout)
	{
		f32 angle = SHIP_BUILDER_CARDINAL_ANGLES[m_spawnCardinalIndex];
		i32 teamId = SHIP_BUILDER_CARDINAL_TEAM_IDS[m_spawnCardinalIndex];
		m_spawnCardinalIndex = (m_spawnCardinalIndex + 1) % 4;

		Vector2double spawnPos{
			SHIP_BUILDER_SPAWN_RADIUS_KM * std::cos(angle),
			SHIP_BUILDER_SPAWN_RADIUS_KM * std::sin(angle)
		};
		f32 facing = static_cast<f32>(std::atan2(-spawnPos.y, -spawnPos.x));

		EntityID ship = m_shipFactory.bake(grid, spawnPos, facing, loadout);
		// mirrors what ShipCollisionTest attaches after bake, bake itself stays policy-free
		m_ecs.addComponent(ship, Faction{ .teamId = teamId });
		m_ecs.addComponent(ship, Separation{ .margin = 0.1f });
		m_ecs.addComponent(ship, AIController{});

		m_saveStatus = "Added to simulation (team " + std::to_string(teamId) + ")";
	}

	void ShipBuilderLayer::refreshSavedShipList()
	{
		m_savedShipNames.clear();
		m_selectedSavedShip = -1;

		std::filesystem::path dir = std::filesystem::path(SOURCE_ROOT) / "Assets" / "Ships";
		if (!std::filesystem::exists(dir)) { return; }

		for (auto& entry : std::filesystem::directory_iterator(dir)) {
			if (entry.path().extension() == ".json") {
				m_savedShipNames.push_back(entry.path().stem().string());
			}
		}
		std::sort(m_savedShipNames.begin(), m_savedShipNames.end());
	}

	void ShipBuilderLayer::drawLoadSection()
	{
		ImGui::Text("Saved Ships");
		ImGui::SameLine();
		if (ImGui::SmallButton("Refresh")) { refreshSavedShipList(); }

		ImGui::BeginChild("ShipBuilderLoadList", ImVec2(0.0f, 90.0f), ImGuiChildFlags_Borders);
		for (i32 i = 0; i < static_cast<i32>(m_savedShipNames.size()); i++) {
			if (ImGui::Selectable(m_savedShipNames[i].c_str(), m_selectedSavedShip == i)) {
				m_selectedSavedShip = i;
			}
		}
		ImGui::EndChild();

		bool hasSelection = m_selectedSavedShip >= 0 && m_selectedSavedShip < static_cast<i32>(m_savedShipNames.size());
		ImGui::BeginDisabled(!hasSelection);
		if (ImGui::Button("Load Selected")) {
			std::filesystem::path path = std::filesystem::path(SOURCE_ROOT) / "Assets" / "Ships" / (m_savedShipNames[m_selectedSavedShip] + ".json");
			std::string loadedName;
			if (loadShipBlueprint(path, m_partRegistry, m_builder, loadedName, getLogger())) {
				m_shipName = loadedName;
				m_shipName.resize(64, '\0');
				m_saveStatus = "Loaded: " + path.string();
			}
			else {
				m_saveStatus = "Load FAILED: " + path.string();
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Spawn Selected")) {
			std::filesystem::path path = std::filesystem::path(SOURCE_ROOT) / "Assets" / "Ships" / (m_savedShipNames[m_selectedSavedShip] + ".json");
			ShipBuilderDesc scratchDesc{ {m_logger}, m_partRegistry, m_builder.grid().width(), m_builder.grid().height() };
			ShipBuilder scratch(scratchDesc);
			std::string loadedName;
			if (loadShipBlueprint(path, m_partRegistry, scratch, loadedName, getLogger())) {
				spawnShip(scratch.grid(), scratch.hardpointLoadout());
			}
			else {
				m_saveStatus = "Spawn FAILED: " + path.string();
			}
		}
		ImGui::EndDisabled();
	}

	void ShipBuilderLayer::drawStatsPanel()
	{
		const ShipBuilderStats& stats = m_builder.stats();

		ImGui::Text("Ship Stats");
		ImGui::Text("Parts: %d", stats.partCount);
		ImGui::Text("Mass: %.1f", stats.totalMass);
		ImGui::Text("Stability: %.1f", stats.totalStability);
		ImGui::Text("Thrust Force: %.1f", stats.totalThrustForce);
		ImGui::Text("Max Accel: %.2f", stats.maxAccel);
		ImGui::Text("System Capacity: %.1f / %.1f", stats.systemCapacityUsed, stats.systemCapacityMax);
		if (stats.isOverSystemCapacity) {
			ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "OVER SYSTEM CAPACITY");
		}

		if (ImGui::Button("Clear Ship")) {
			m_builder.clear();
		}
	}

	void ShipBuilderLayer::draw()
	{
		ImGui::InputText("Ship Name", m_shipName.data(), m_shipName.capacity() + 1);
		ImGui::Separator();
		drawPalette();
		ImGui::Separator();
		drawGrid();
		ImGui::Separator();
		drawHardpointLoadout();
		ImGui::Separator();
		drawStatsPanel();
		ImGui::Separator();
		drawLoadSection();
		ImGui::Separator();

		if (ImGui::Button("Save Ship")) { save(); }
		if (!m_saveStatus.empty()) { ImGui::TextUnformatted(m_saveStatus.c_str()); }
	}
}
