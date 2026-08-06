#pragma once
#include <Core/Base.h>
#include <Ships/ShipGrid.h>
#include <Ships/PartRegistry.h>
#include <Ships/PartVariant.h>
#include <Ships/PartTypes.h>
#include <Ships/PairHash.h>

namespace Engine {
	struct ShipBuilderStats {
		f32 totalMass{ 0.0f };
		f32 totalStability{ 0.0f };
		f32 totalThrustForce{ 0.0f };
		f32 maxAccel{ 0.0f };
		f32 systemCapacityMax{ 0.0f };
		f32 systemCapacityUsed{ 0.0f };
		bool isOverSystemCapacity{ false };
		f32 totalSignalEmission{ 0.0f };
		f32 totalSensorPower{ 0.0f };
		i32 partCount{ 0 };
	};

	struct ShipBuilderDesc {
		BaseDesc base;
		PartRegistry& partRegistry;
		i32 gridWidth;
		i32 gridHeight;
	};

	class ShipBuilder : public Base {
	public:
		explicit ShipBuilder(const ShipBuilderDesc& desc);

		[[nodiscard]] const ShipGrid& grid() const noexcept { return m_grid; }
		[[nodiscard]] const ShipBuilderStats& stats() const noexcept { return m_stats; }
		[[nodiscard]] const HardpointLoadout& hardpointLoadout() const noexcept { return m_hardpointLoadout; }

		bool placePart(i32 x, i32 y, PartVariantID variant);
		void removePart(i32 x, i32 y);
		void clear();

		[[nodiscard]] PartVariantID weaponAt(i32 x, i32 y) const;
		bool assignWeapon(i32 x, i32 y, PartVariantID weaponVariant);
		void clearWeapon(i32 x, i32 y);

	private:
		void recomputeStats();

		PartRegistry& m_partRegistry;
		ShipGrid m_grid;
		ShipBuilderStats m_stats;
		HardpointLoadout m_hardpointLoadout;
	};
}
