#include <Ships/ShipBuilder.h>

namespace Engine {
	constexpr f32 SHIP_BUILDER_THRUST_FORCE_MULTIPLIER = 2.0f;

	ShipBuilder::ShipBuilder(const ShipBuilderDesc& desc) : Base(desc.base),
		m_partRegistry(desc.partRegistry),
		m_grid(ShipGridDesc{ desc.base, desc.gridWidth, desc.gridHeight })
	{
		recomputeStats();
	}

	bool ShipBuilder::placePart(i32 x, i32 y, PartVariantID variant)
	{
		if (variant == INVALID_PART_VARIANT) { return false; }

		const PartVariant& partVariant = m_partRegistry.get(variant);

		i32 sizeX = 1, sizeY = 1;
		if (partVariant.category == PartCategory::Hardpoint) {
			const HardpointParams& hardpointParams = std::get<HardpointParams>(partVariant.params);
			sizeX = hardpointParams.sizeX;
			sizeY = hardpointParams.sizeY;
		}

		if (!m_grid.tryPlacePart(x, y, sizeX, sizeY, partVariant.category, variant)) { return false; }

		recomputeStats();
		return true;
	}

	void ShipBuilder::removePart(i32 x, i32 y)
	{
		const GridCell& clicked = m_grid.at(x, y);
		if (clicked.partCategory == PartCategory::Hardpoint) {
			i32 anchorX = clicked.isAnchor ? x : clicked.anchorX;
			i32 anchorY = clicked.isAnchor ? y : clicked.anchorY;
			m_hardpointLoadout.erase({ anchorX, anchorY });
		}

		m_grid.removePart(x, y);
		recomputeStats();
	}

	void ShipBuilder::clear()
	{
		for (i32 y = 0; y < m_grid.height(); y++) {
			for (i32 x = 0; x < m_grid.width(); x++) {
				const GridCell& cell = m_grid.at(x, y);
				if (cell.partCategory != PartCategory::None && cell.isAnchor) {
					m_grid.removePart(x, y);
				}
			}
		}
		m_hardpointLoadout.clear();
		recomputeStats();
	}

	PartVariantID ShipBuilder::weaponAt(i32 x, i32 y) const
	{
		const GridCell& clicked = m_grid.at(x, y);
		if (clicked.partCategory != PartCategory::Hardpoint) { return INVALID_PART_VARIANT; }

		i32 anchorX = clicked.isAnchor ? x : clicked.anchorX;
		i32 anchorY = clicked.isAnchor ? y : clicked.anchorY;

		auto it = m_hardpointLoadout.find({ anchorX, anchorY });
		return it != m_hardpointLoadout.end() ? it->second : INVALID_PART_VARIANT;
	}

	bool ShipBuilder::assignWeapon(i32 x, i32 y, PartVariantID weaponVariant)
	{
		const GridCell& clicked = m_grid.at(x, y);
		if (clicked.partCategory != PartCategory::Hardpoint) { return false; }
		if (weaponVariant == INVALID_PART_VARIANT) { return false; }
		if (m_partRegistry.get(weaponVariant).category != PartCategory::Weapon) { return false; }

		i32 anchorX = clicked.isAnchor ? x : clicked.anchorX;
		i32 anchorY = clicked.isAnchor ? y : clicked.anchorY;

		m_hardpointLoadout[{ anchorX, anchorY }] = weaponVariant;
		recomputeStats();
		return true;
	}

	void ShipBuilder::clearWeapon(i32 x, i32 y)
	{
		const GridCell& clicked = m_grid.at(x, y);
		if (clicked.partCategory != PartCategory::Hardpoint) { return; }

		i32 anchorX = clicked.isAnchor ? x : clicked.anchorX;
		i32 anchorY = clicked.isAnchor ? y : clicked.anchorY;

		if (m_hardpointLoadout.erase({ anchorX, anchorY }) > 0) {
			recomputeStats();
		}
	}

	void ShipBuilder::recomputeStats()
	{
		ShipBuilderStats stats{};

		auto applySystemCapacity = [&](f32 contribution) {
			if (contribution > 0.0f) { stats.systemCapacityMax += contribution; }
			else { stats.systemCapacityUsed += -contribution; }
			};

		for (i32 y = 0; y < m_grid.height(); y++) {
			for (i32 x = 0; x < m_grid.width(); x++) {
				const GridCell& cell = m_grid.at(x, y);
				if (cell.partCategory == PartCategory::None || !cell.isAnchor) { continue; }

				stats.partCount++;

				const PartVariant& variant = m_partRegistry.get(cell.variant);
				std::visit([&](auto&& params) {
					using T = std::decay_t<decltype(params)>;
					stats.totalMass += params.mass;
					stats.totalStability += params.stabilityContribution;
					applySystemCapacity(params.systemCapacityContribution);
					stats.totalSignalEmission += params.signalEmissionValue;
					stats.totalSensorPower += params.sensorPowerValue;
					if constexpr (std::is_same_v<T, EngineParams>) {
						stats.totalThrustForce += params.thrustForce;
					}
					}, variant.params);

				if (cell.partCategory == PartCategory::Hardpoint) {
					auto loadoutIt = m_hardpointLoadout.find({ x, y });
					if (loadoutIt != m_hardpointLoadout.end()) {
						const WeaponParams& weaponParams = std::get<WeaponParams>(m_partRegistry.get(loadoutIt->second).params);
						applySystemCapacity(weaponParams.systemCapacityContribution);
						stats.totalSignalEmission += weaponParams.signalEmissionValue;
						stats.totalSensorPower += weaponParams.sensorPowerValue;
					}
				}
			}
		}

		stats.isOverSystemCapacity = stats.systemCapacityUsed > stats.systemCapacityMax;
		stats.maxAccel = (stats.totalThrustForce > 0.0f && stats.totalMass > 0.0f)
			? (stats.totalThrustForce * SHIP_BUILDER_THRUST_FORCE_MULTIPLIER) / stats.totalMass
			: 0.0f;

		m_stats = stats;
	}
}
