#include <Ships/PartRegistry.h>

namespace Engine {
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
	Weapon PartRegistry::buildWeaponFromVariant(PartVariantID variantId) const
	{
		const PartVariant& variant = get(variantId);
		ENGINE_ASSERT(variant.category == PartCategory::Weapon, "buildWeaponFromVariant: variant is not a Weapon");

		const WeaponParams& params = std::get<WeaponParams>(variant.params);
		return Weapon{
			.cooldown = params.cooldown,
			.timeSinceLastFire = 0.0f,
			.projectileSpeed = params.projectileSpeed,
			.projectileRadius = params.projectileRadius,
			.projectileDamage = params.damage
		};
	}
}