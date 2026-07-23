#pragma once
#include <Core/Common.h>
#include <GameECS.h>
#include <ECS/TickedSystem.h>
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

	private:
		void applyHit(EntityID damageDealer, EntityID target);

		GameECSWrapper& m_ecs;
		CollisionSystem& m_collisionSystem;

		std::bitset<64> m_healthMask{};
		std::bitset<64> m_damageMask{};
	};
}