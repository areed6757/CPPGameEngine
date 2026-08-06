#pragma once
#include <Game.h>

namespace Engine {
	struct AISystemDesc {
		BaseDesc base;
		GameECSWrapper& ecs;
	};

	// Throwaway rudimentary AI: turn-to-face, close within weapon range then hold, plus a small
	// separation force so same-team ships don't stack on each other. No pathfinding, no utility
	// scoring, no formation awareness, not meant to be efficient yet. Targeting itself (who, if
	// anyone) is resolved by DetectionSystem against the signal tree before this system runs -
	// AIController::target/targetResolution arrive here already decided.
	class AISystem : public Base, public TickedSystem {
	public:
		explicit AISystem(const AISystemDesc& desc);
		~AISystem();

		void Update(d64 dt) override;

	private:
		GameECSWrapper& m_ecs;
		std::bitset<64> m_entityMask{};
	};
}