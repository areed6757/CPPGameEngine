#pragma once
#include <Core/Base.h>
#include <Core/Common.h>
#include <Physics/Vector2float.h>
#include <Physics/Vector2double.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>

namespace Engine {
	using Json = nlohmann::json;

	inline void to_json(Json& j, const Vector2float& v) { j = Json{ {"x", v.x}, {"y", v.y} }; }
	inline void from_json(const Json& j, Vector2float& v) { j.at("x").get_to(v.x); j.at("y").get_to(v.y); }

	inline void to_json(Json& j, const Vector2double& v) { j = Json{ {"x", v.x}, {"y", v.y} }; }
	inline void from_json(const Json& j, Vector2double& v) { j.at("x").get_to(v.x); j.at("y").get_to(v.y); }

	[[nodiscard]] inline bool writeJsonFile(const std::filesystem::path& path, const Json& j) {
		std::ofstream out(path);
		if (!out.is_open()) { return false; }
		out << j.dump(2); // pretty-printed, human-editable -- worth it for a data format artists/designers will hand-tweak
		return true;
	}

	[[nodiscard]] inline bool readJsonFile(const std::filesystem::path& path, Json& outJson) {
		std::ifstream in(path);
		if (!in.is_open()) { return false; }
		try {
			in >> outJson;
		}
		catch (const Json::parse_error&) {
			return false;
		}
		return true;
	}
}