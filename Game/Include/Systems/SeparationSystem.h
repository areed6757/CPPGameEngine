#pragma once
#include <Game.h>

namespace Engine {
	struct SeparationSystemDesc {
		BaseDesc base;
		GameECSWrapper& ecs;
		AABBTree& aabbTree;
	};

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

		f32 m_maxRadiusSeenThisTick{ 0.0f };
	};
}