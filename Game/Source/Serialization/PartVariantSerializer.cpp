#include <Serialization/PartVariantSerializer.h>

namespace Engine {
	namespace {
		Json serializeBaseStats(const PartBaseStats& s) {
			return Json{ {"mass", s.mass}, {"health", s.health},
				{"stabilityContribution", s.stabilityContribution}, {"kineticResistance", s.kineticResistance},
				{"systemCapacityContribution", s.systemCapacityContribution},
				{"signalEmissionValue", s.signalEmissionValue} };
		}
		PartBaseStats deserializeBaseStats(const Json& j) {
			return PartBaseStats{
				j.at("mass").get<f32>(), j.at("health").get<f32>(),
				j.at("stabilityContribution").get<f32>(), j.at("kineticResistance").get<f32>(),
				j.value("systemCapacityContribution", 0.0f),
				j.value("signalEmissionValue", 0.0f)
			};
		}
	}

	Json serializePartVariant(const PartVariant& variant)
	{
		Json j;
		j["name"] = variant.name;
		j["category"] = static_cast<i32>(variant.category);

		std::visit([&](auto&& params) {
			using T = std::decay_t<decltype(params)>;
			Json base = serializeBaseStats(params); // every Params type inherits PartBaseStats

			if constexpr (std::is_same_v<T, HullParams>) {
				j["paramsType"] = "Hull";
			}
			else if constexpr (std::is_same_v<T, ArmorParams>) {
				j["paramsType"] = "Armor";
			}
			else if constexpr (std::is_same_v<T, EngineParams>) {
				j["paramsType"] = "Engine";
				base["thrustForce"] = params.thrustForce;
				base["powerDraw"] = params.powerDraw;
			}
			else if constexpr (std::is_same_v<T, WeaponParams>) {
				j["paramsType"] = "Weapon";
				base["damage"] = params.damage;
				base["cooldown"] = params.cooldown;
				base["projectileSpeed"] = params.projectileSpeed;
				base["projectileRadius"] = params.projectileRadius;
				base["projectileLifetime"] = params.projectileLifetime;
				base["barrelCount"] = params.barrelCount;
				base["barrelSpread"] = params.barrelSpread;
				base["muzzleForwardOffset"] = params.muzzleForwardOffset;
				base["anchorOffsetX"] = params.anchorOffset.x;
				base["anchorOffsetY"] = params.anchorOffset.y;
				base["minRotation"] = params.minRotation;
				base["maxRotation"] = params.maxRotation;
				base["traverseSpeed"] = params.traverseSpeed;
				base["accuracy"] = params.accuracy;
				base["role"] = static_cast<i32>(params.role);
			}
			else if constexpr (std::is_same_v<T, HardpointParams>) {
				j["paramsType"] = "Hardpoint";
				base["sizeX"] = params.sizeX;
				base["sizeY"] = params.sizeY;
			}
			j["params"] = base;
			}, variant.params);

		return j;
	}

	bool deserializePartVariant(const Json& j, PartVariant& outVariant)
	{
		try {
			outVariant.name = j.at("name").get<std::string>();
			outVariant.category = static_cast<PartCategory>(j.at("category").get<i32>());

			std::string paramsType = j.at("paramsType").get<std::string>();
			const Json& p = j.at("params");
			PartBaseStats base = deserializeBaseStats(p);

			if (paramsType == "Hull") {
				outVariant.params = HullParams{ base };
			}
			else if (paramsType == "Armor") {
				outVariant.params = ArmorParams{ base };
			}
			else if (paramsType == "Engine") {
				outVariant.params = EngineParams{ base, p.at("thrustForce").get<f32>(), p.at("powerDraw").get<f32>() };
			}
			else if (paramsType == "Weapon") {
				outVariant.params = WeaponParams{ base,
					p.at("damage").get<f32>(), p.at("cooldown").get<f32>(),
					p.at("projectileSpeed").get<f32>(), p.at("projectileRadius").get<f32>(),
					p.value("projectileLifetime", 3.0f),
					p.value("barrelCount", 1), p.value("barrelSpread", 0.0f),
					p.value("muzzleForwardOffset", 0.0f),
					Vector2float{ p.value("anchorOffsetX", 0.0f), p.value("anchorOffsetY", 0.0f) },
					p.value("minRotation", -PI), p.value("maxRotation", PI),
					p.value("traverseSpeed", 1000.0f), p.value("accuracy", 0.0f),
					static_cast<WeaponRole>(p.value("role", static_cast<i32>(WeaponRole::StandardPrimary))) };
			}
			else if (paramsType == "Hardpoint") {
				outVariant.params = HardpointParams{ base, p.at("sizeX").get<i32>(), p.at("sizeY").get<i32>() };
			}
			else {
				return false;
			}
			return true;
		}
		catch (const Json::exception& e) {
			return false;
		}
	}

	bool savePartVariant(const PartVariant& variant, const std::filesystem::path& path)
	{
		return writeJsonFile(path, serializePartVariant(variant));
	}

	bool loadPartVariant(const std::filesystem::path& path, PartVariant& outVariant)
	{
		Json j;
		if (!readJsonFile(path, j)) { return false; }
		return deserializePartVariant(j, outVariant);
	}
}