#pragma once
#include <Game.h>

namespace Engine {
	struct DamperSystemDesc {
		BaseDesc base;
		GameECSWrapper& ecs;
	};
	
	class DamperSystem : public Base, public TickedSystem {
	public:
		explicit DamperSystem(const DamperSystemDesc& desc);
		~DamperSystem();

		void Update(d64 dt) override;

	private:
		GameECSWrapper& m_ecs;

		std::bitset<64> m_entityMask;
	};
}