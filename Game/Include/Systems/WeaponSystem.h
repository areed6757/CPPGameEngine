#pragma once
#include <Game.h>

namespace Engine {
	struct WeaponSystemDesc {
		BaseDesc base;
		GameECSWrapper& ecs;

	};

	class WeaponSystem : public Base, public TickedSystem {
	public:
		explicit WeaponSystem(const WeaponSystemDesc& desc);
		~WeaponSystem();

		void Update(d64 dt) override;

		GameCommandBuffer& getCommandBuffer() noexcept { return m_cmdBuffer; }

		void fire(EntityID id);

	private:
		GameECSWrapper& m_ecs;
		GameCommandBuffer m_cmdBuffer;
		std::bitset<64> m_entityMask{};
	};
}