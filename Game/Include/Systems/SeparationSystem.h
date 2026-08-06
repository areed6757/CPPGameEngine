#pragma once
#include <Game.h>
#include <Physics/ORCA.h>

namespace Engine {
	struct SeparationSystemDesc {
		BaseDesc base;
		GameECSWrapper& ecs;
		AABBTree& aabbTree;
	};

	// local collision avoidance among same-team ships via ORCA, anticipates closing velocity over Separation::timeHorizon instead of only reacting once ships already overlap
	class SeparationSystem : public Base, public TickedSystem {
	public:
		explicit SeparationSystem(const SeparationSystemDesc& desc);
		~SeparationSystem();

		void Update(d64 dt) override;

	private:
		GameECSWrapper& m_ecs;
		AABBTree& m_aabbTree;
		std::bitset<64> m_entityMask{};
		std::vector<EntityID> m_nearbyScratch;
		std::vector<ORCALine> m_orcaLines;

		f32 m_maxRadiusSeenThisTick{ 0.0f };
		f32 m_maxSpeedSeenThisTick{ 0.0f };
	};
}