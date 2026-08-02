#pragma once
#include <Engine.h>
#include <Ships/PartVariant.h>
#include <filesystem>

namespace Engine {
	[[nodiscard]] Json serializePartVariant(const PartVariant& variant);
	[[nodiscard]] bool deserializePartVariant(const Json& j, PartVariant& outVariant);

	[[nodiscard]] bool savePartVariant(const PartVariant& variant, const std::filesystem::path& path);
	[[nodiscard]] bool loadPartVariant(const std::filesystem::path& path, PartVariant& outVariant);
}