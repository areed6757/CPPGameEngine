#pragma once
#include <Core/Base.h>
#include <Core/Common.h>
#include <GameECS.h>
#include <Ships/ShipGrid.h>
#include <Ships/PartRegistry.h>
#include <Ships/PairHash.h>
#include <unordered_map>

namespace Engine {
	struct ShipFactoryDesc {
		BaseDesc base;
		GameECSWrapper& ecs;
		PartRegistry& partReg;
	};

	// Bakes a ShipGrid into a real ship: one entity aggregating Hull/Armor/Engine/Hardpoint stats (mass, stability, thrust), a ShipVisual carrying
	// per-part draw data for those same categories (no per-cell entities), and one Mount entity per hardpoint (bare socket unless a weapon variant
	// is assigned via hardpointLoadout, in which case it also gets a real Weapon component built from that variant).
	class ShipFactory : public Base {
	public:
		explicit ShipFactory(const ShipFactoryDesc& desc);
		~ShipFactory();

		EntityID bake(const ShipGrid& grid, Vector2double spawnPos, f32 spawnRotation,
			const std::unordered_map<std::pair<i32, i32>, PartVariantID, PairHash>& hardpointLoadout);

	private:
		GameECSWrapper& m_ecs;
		PartRegistry& m_partReg;
	};
}