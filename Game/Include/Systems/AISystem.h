#pragma once
#include <Game.h>

namespace Engine {
	struct AISystemDesc {
		BaseDesc base;
		GameECSWrapper& ecs;
	};

	// Throwaway rudimentary AI: nearest-enemy targeting, turn-to-face,
	// close within weapon range then hold, plus a small separation force so
	// same-team ships don't stack on each other. No pathfinding, no
	// utility scoring, no formation awareness, O(n^2) targetting, not meant to be efficient yet
	class AISystem : public Base, public TickedSystem {
	public:
		explicit AISystem(const AISystemDesc& desc);
		~AISystem();

		void Update(d64 dt) override;

	private:
		EntityID findNearestEnemy(EntityID self, i32 selfTeam, const Vector2double& selfPos) const;

		GameECSWrapper& m_ecs;
		std::bitset<64> m_entityMask{};
	};
}