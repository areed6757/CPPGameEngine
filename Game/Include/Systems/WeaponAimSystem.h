#pragma once
#include <Game.h>

namespace Engine {
	struct WeaponAimSystemDesc {
		BaseDesc base;
		GameECSWrapper& ecs;
		AABBTree& aabbTree;
	};

	// AI-target-only weapon aiming: finds the nearest enemy from the weapon's
	// mount anchor, leads the intercept point, and steps aimRotation toward the
	// clamped ship-relative angle at the weapon's traverseSpeed. No player/mouse
	// aiming - an explicit scope decision for this pass.
	class WeaponAimSystem : public Base, public TickedSystem {
	public:
		explicit WeaponAimSystem(const WeaponAimSystemDesc& desc);
		~WeaponAimSystem();

		void Update(d64 dt) override;

	private:
		EntityID findNearestEnemy(EntityID self, i32 selfTeam, const Vector2double& selfPos) const;
		Vector2double predictInterceptPoint(const Vector2double& selfPos, f32 projectileSpeed,
			const Vector2double& targetPos, const Vector2float& targetVel, d64 dist) const;

		GameECSWrapper& m_ecs;
		AABBTree& m_aabbTree;
		std::bitset<64> m_entityMask{};
		mutable std::vector<EntityID> m_nearbyScratch;
	};
}
