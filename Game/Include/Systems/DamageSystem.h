#pragma once
#include <Engine.h>
#include <GameECS.h>
#include <Systems/CollisionSystem.h>

namespace Engine {
	struct DamageSystemDesc {
		BaseDesc base;
		GameECSWrapper& ecs;
		CollisionSystem& collisionSystem;
	};

	class DamageSystem : public Base, public TickedSystem {
	public:
		explicit DamageSystem(const DamageSystemDesc& desc);
		~DamageSystem();

		void Update(d64 dt) override;

		GameCommandBuffer& getCommandBuffer() noexcept { return m_cmdBuffer; }


	private:
		void applyHit(EntityID damageDealer, EntityID target);

		GameECSWrapper& m_ecs;
		GameCommandBuffer m_cmdBuffer;
		CollisionSystem& m_collisionSystem;

		std::bitset<64> m_healthMask{};
		std::bitset<64> m_damageMask{};
	};
}