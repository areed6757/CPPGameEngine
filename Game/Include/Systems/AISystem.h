#pragma once
#include <Game.h>

namespace Engine {
	struct AISystemDesc {
		BaseDesc base;
		GameECSWrapper& ecs;
		AABBTree& aabbTree;
	};

	// Throwaway rudimentary AI: nearest-enemy targeting, turn-to-face,
	// close within weapon range then hold, plus a small separation force so
	// same-team ships don't stack on each other. No pathfinding, no
	// utility scoring, no formation awareness, not meant to be efficient yet.
	// findNearestEnemy routes through AABBTree (expanding-radius query) rather
	// than the O(n^2) brute-force scan it used to be.
	class AISystem : public Base, public TickedSystem {
	public:
		explicit AISystem(const AISystemDesc& desc);
		~AISystem();

		void Update(d64 dt) override;

	private:
		EntityID findNearestEnemy(EntityID self, i32 selfTeam, const Vector2double& selfPos) const;

		GameECSWrapper& m_ecs;
		AABBTree& m_aabbTree;
		std::bitset<64> m_entityMask{};
		mutable std::vector<EntityID> m_nearbyScratch;
	};
}