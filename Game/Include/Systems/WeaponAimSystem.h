#pragma once
#include <Game.h>
#include <random>

namespace Engine {
	struct WeaponAimSystemDesc {
		BaseDesc base;
		GameECSWrapper& ecs;
	};

	// AI-target-only weapon aiming: reads the mount owner's already-resolved signal target
	// (AIController::target/targetResolution, set earlier this tick by DetectionSystem), leads
	// the intercept point off a resolution-noised anchor (a random point in a shrinking radius
	// around the target's real position, not the exact position itself), and steps aimRotation
	// toward the clamped ship-relative angle at the weapon's traverseSpeed. Weapon::accuracy's
	// own resolution scaling still happens separately at fire time (WeaponSystem). No player/
	// mouse aiming - an explicit scope decision for this pass.
	class WeaponAimSystem : public Base, public TickedSystem {
	public:
		explicit WeaponAimSystem(const WeaponAimSystemDesc& desc);
		~WeaponAimSystem();

		void Update(d64 dt) override;

	private:
		Vector2double predictInterceptPoint(const Vector2double& selfPos, f32 projectileSpeed,
			const Vector2double& targetPos, const Vector2float& targetVel, d64 dist) const;

		GameECSWrapper& m_ecs;
		std::mt19937 m_rng;
		std::bitset<64> m_entityMask{};
	};
}
