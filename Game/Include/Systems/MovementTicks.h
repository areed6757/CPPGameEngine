#pragma once
#include <Game.h>
#include <bitset>
#include <format>

namespace Engine {
	struct MovementTicksDesc {
		BaseDesc base;
		GameECSWrapper& ecs;
		ThreadPool& threadPool;
	};

	class MovementTicks final : public Base, public TickedSystem {
	public:
		explicit MovementTicks(const MovementTicksDesc& desc);
		~MovementTicks();

		void Update(d64 deltaTime) override;

		std::vector<Job> buildJobs(d64 dt) override;

	private:
		GameECSWrapper& m_ecs;
		std::bitset<64> m_entityMask;
		i32 m_tickCount = 0;

		void updateRange(i32 start, i32 end, d64 dt);

		i32 m_chunkCount;
	};
}