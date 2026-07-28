#pragma once
#include <Game.h>

namespace Engine {
	struct ShipFactoryDesc {
		BaseDesc base;
		GameECSWrapper& ecs;
		PartRegistry& partReg;
	};

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