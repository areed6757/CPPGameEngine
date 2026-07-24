#pragma once
#include <Components/Movement.h>
#include <Components/Position.h>
#include <Core/Common.h>
#include <Systems/CollisionSystem.h>
#include <ECS/TickedSystem.h>
#include <Utilities/Job.h>
#include <Utilities/ThreadPool.h>
#include <GameDescs.h>
#include <GameECS.h>
#include <bitset>
#include <format>

namespace Engine {
	struct MovementTicksDesc {
		BaseDesc base;
		GameECSWrapper& ecs;
		CollisionSystem& collisionSystem;
		ThreadPool& threadPool;
	};

	class MovementTicks final : public Base, public TickedSystem {
	public:
		explicit MovementTicks(const MovementTicksDesc& desc);
		~MovementTicks();

		void Update(d64 deltaTime) override;

		std::vector<Job> buildJobs(d64 dt) override;

		std::bitset<64> getReadSignature() const noexcept { return m_reads; }
		std::bitset<64> getWriteSignature() const noexcept { return m_writes; }

	protected:
		std::bitset<64> m_reads{};
		std::bitset<64> m_writes{};

	private:
		GameECSWrapper& m_ecs;
		std::bitset<64> m_entityMask;
		i32 m_tickCount = 0;
		CollisionSystem& m_collisionSystem;

		void drainImpulses();
		void updateRange(i32 start, i32 end, d64 dt);

		i32 m_chunkCount;
	};
}