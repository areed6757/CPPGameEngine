#include <Ships/PartRegistry.h>
#include <cmath>

namespace Engine {
	constexpr f32 WEAPON_NOMINAL_TARGET_RADIUS_KM = 0.03f;

	PartRegistry::PartRegistry(const PartRegistryDesc& desc) : Base(desc.base)
	{
		EngineLogInfo("Part registry created.");
	}

	PartRegistry::~PartRegistry()
	{
		EngineLogInfo("Part registry destroyed.");
	}

	PartVariantID PartRegistry::registerVariant(const PartVariant& variant)
	{
		m_variants.push_back(variant);
		PartVariantID id = static_cast<PartVariantID>(m_variants.size() - 1);
		EngineLogInfo("Registered part variant '{}' as id {}", variant.name, id);
		return id;
	}

	const PartVariant& PartRegistry::get(PartVariantID id) const
	{
		ENGINE_ASSERT(id >= 0 && id < static_cast<PartVariantID>(m_variants.size()), "PartRegistry::get: invalid id");
		return m_variants[id];
	}
	PartVariantID PartRegistry::findByName(const std::string& name) const noexcept
	{
		for (i32 i = 0; i < static_cast<i32>(m_variants.size()); i++) {
			if (m_variants[i].name == name) { return i; }
		}
		return INVALID_PART_VARIANT;
	}

	Weapon PartRegistry::buildWeaponFromVariant(PartVariantID variantId, f32 mountRotation) const
	{
		const WeaponParams& params = std::get<WeaponParams>(get(variantId).params);
		Weapon weapon{
			.cooldown = params.cooldown, .timeSinceLastFire = 0.0f,
			.projectileSpeed = params.projectileSpeed, .projectileRadius = params.projectileRadius,
			.projectileDamage = params.damage, .projectileLifetime = params.projectileLifetime,
			.barrelCount = std::clamp(params.barrelCount, 1, 4),
			.traverseSpeed = params.traverseSpeed, .accuracy = params.accuracy
		};
		f32 half = (weapon.barrelCount - 1) * 0.5f;
		for (i32 i = 0; i < weapon.barrelCount; i++) {
			weapon.barrelOffsets[i] = Vector2float{ params.muzzleForwardOffset, (static_cast<f32>(i) - half) * params.barrelSpread };
		}
		weapon.minRotationAbs = mountRotation + params.minRotation;
		weapon.maxRotationAbs = mountRotation + params.maxRotation;
		weapon.restRotationAbs = (weapon.minRotationAbs + weapon.maxRotationAbs) * 0.5f;
		weapon.aimRotation = weapon.restRotationAbs;

		weapon.maxRange = params.projectileSpeed * params.projectileLifetime;
		weapon.idealRange = params.accuracy > 0.0f
			? std::min(weapon.maxRange, WEAPON_NOMINAL_TARGET_RADIUS_KM / std::tan(params.accuracy))
			: weapon.maxRange;
		weapon.role = params.role;
		return weapon;
	}

	PartVariantID PartRegistry::loadVariantFromFile(const std::filesystem::path& path)
	{
		PartVariant variant;
		if (!loadPartVariant(path, variant)) {
			EngineLogError("PartRegistry::loadVariantFromFile: failed to load {}", path.string());
			return INVALID_PART_VARIANT;
		}
		return registerVariant(variant);
	}

	i32 PartRegistry::loadAllFromDirectory(const std::filesystem::path& dir)
	{
		i32 count = 0;
		if (!std::filesystem::exists(dir)) { return count; }
		for (auto& entry : std::filesystem::directory_iterator(dir)) {
			EngineLogInfo("Found entry: {}", entry.path().string());
			
			if (entry.path().extension() != ".json") { continue; }
			if (loadVariantFromFile(entry.path()) != INVALID_PART_VARIANT) { count++; }
		}
		return count;
	}
}