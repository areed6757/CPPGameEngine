#pragma once
#include <Game.h>

namespace Engine {
	class ShipGrid;
	class PartRegistry;

	struct ShipFactoryDesc {
		BaseDesc base;
		GameECSWrapper& ecs;
		PartRegistry& partReg;
	};


	// Bakes a ShipGrid into a real ship: one entity aggregating Hull/Armor/Engine/Hardpoint 
	// stats (mass, stability, thrust), a ShipVisual carrying per-part draw data for those
	// same categories, and one real weapon + weaponmount per hardpoiint
	class ShipFactory : public Base {
	public:
		explicit ShipFactory(const ShipFactoryDesc& desc);
		~ShipFactory();

		EntityID bake(const ShipGrid& grid, Vector2double spawnPos, f32 spawnRotation);

	private:
		GameECSWrapper& m_ecs;
		PartRegistry& m_partReg;
	};
}