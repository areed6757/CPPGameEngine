#include <Serialization/ShipBlueprintSerializer.h>

namespace Engine {
	Json serializeShipBlueprint(const std::string& name, const ShipGrid& grid,
		const PartRegistry& partReg, const HardpointLoadout& loadout)
	{
		Json j;
		j["name"] = name;
		j["width"] = grid.width();
		j["height"] = grid.height();

		Json parts = Json::array();
		for (i32 y = 0; y < grid.height(); y++) {
			for (i32 x = 0; x < grid.width(); x++) {
				const GridCell& cell = grid.at(x, y);
				if (cell.partCategory == PartCategory::None || !cell.isAnchor) { continue; }

				parts.push_back(Json{
					{"x", x}, {"y", y},
					{"part", partReg.get(cell.variant).name},
					{"rotation", cell.rotation}
					});
			}
		}
		j["parts"] = parts;

		Json loadoutJson = Json::array();
		for (auto& [coord, weaponVariant] : loadout) {
			loadoutJson.push_back(Json{
				{"x", coord.first}, {"y", coord.second},
				{"weapon", partReg.get(weaponVariant).name}
				});
		}
		j["hardpointLoadout"] = loadoutJson;

		return j;
	}

	bool saveShipBlueprint(const std::string& name, const ShipGrid& grid,
		const PartRegistry& partReg, const HardpointLoadout& loadout, const std::filesystem::path& path)
	{
		return writeJsonFile(path, serializeShipBlueprint(name, grid, partReg, loadout));
	}

	bool deserializeShipBlueprint(const Json& j, PartRegistry& partReg, ShipBuilder& builder, std::string& outName, Logger& logger)
	{
		// shadows the unqualified getLogger() that EngineLogWarning expands to
		auto getLogger = [&logger]() -> Logger& { return logger; };

		try {
			outName = j.at("name").get<std::string>();

			builder.clear();

			for (auto& p : j.at("parts")) {
				i32 x = p.at("x").get<i32>();
				i32 y = p.at("y").get<i32>();
				std::string partName = p.at("part").get<std::string>();

				PartVariantID variant = partReg.findByName(partName);
				if (variant == INVALID_PART_VARIANT) {
					EngineLogWarning("ShipBlueprint: unknown part '{}' at ({}, {}), skipping", partName, x, y);
					continue;
				}
				if (!builder.placePart(x, y, variant)) {
					EngineLogWarning("ShipBlueprint: failed to place '{}' at ({}, {})", partName, x, y);
				}
			}

			if (j.contains("hardpointLoadout")) {
				for (auto& entry : j.at("hardpointLoadout")) {
					i32 x = entry.at("x").get<i32>();
					i32 y = entry.at("y").get<i32>();
					std::string weaponName = entry.at("weapon").get<std::string>();

					PartVariantID weaponVariant = partReg.findByName(weaponName);
					if (weaponVariant == INVALID_PART_VARIANT) {
						EngineLogWarning("ShipBlueprint: unknown weapon '{}' for hardpoint ({}, {}), skipping", weaponName, x, y);
						continue;
					}
					builder.assignWeapon(x, y, weaponVariant);
				}
			}

			return true;
		}
		catch (const Json::exception&) {
			return false;
		}
	}

	bool loadShipBlueprint(const std::filesystem::path& path, PartRegistry& partReg, ShipBuilder& builder, std::string& outName, Logger& logger)
	{
		Json j;
		if (!readJsonFile(path, j)) { return false; }
		return deserializeShipBlueprint(j, partReg, builder, outName, logger);
	}
}
