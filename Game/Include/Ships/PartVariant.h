#pragma once
#include <Ships/PartTypes.h>
#include <Core/Common.h>
#include <string>
#include <variant>

namespace Engine {
	using PartVariantID = i32;
	constexpr PartVariantID INVALID_PART_VARIANT = -1;

	struct PartVariant {
		std::string name;
		PartCategory category;
		std::variant<HullParams, ArmorParams, EngineParams, HardpointParams, WeaponParams> params;
		std::optional<TextureID> texture;
	};
}
