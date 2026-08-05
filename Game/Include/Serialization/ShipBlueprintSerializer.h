#pragma once
#include <Engine.h>
#include <Ships/ShipGrid.h>
#include <Ships/ShipBuilder.h>
#include <Ships/PartRegistry.h>
#include <Ships/PairHash.h>
#include <filesystem>
#include <string>

namespace Engine {
	// parts are referenced by name, not PartVariantID, since ids aren't stable across sessions
	[[nodiscard]] Json serializeShipBlueprint(const std::string& name, const ShipGrid& grid,
		const PartRegistry& partReg, const HardpointLoadout& loadout);

	[[nodiscard]] bool saveShipBlueprint(const std::string& name, const ShipGrid& grid,
		const PartRegistry& partReg, const HardpointLoadout& loadout, const std::filesystem::path& path);

	// free functions have no Base to supply getLogger, so the caller passes one in
	[[nodiscard]] bool deserializeShipBlueprint(const Json& j, PartRegistry& partReg, ShipBuilder& builder, std::string& outName, Logger& logger);
	[[nodiscard]] bool loadShipBlueprint(const std::filesystem::path& path, PartRegistry& partReg, ShipBuilder& builder, std::string& outName, Logger& logger);
}
