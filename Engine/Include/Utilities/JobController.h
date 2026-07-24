#pragma once
#include <Core/Common.h>
#include <ECS/TickedSystem.h>
#include <Utilities/ThreadPool.h>
#include <Utilities/Job.h>
#include <vector>
#include <unordered_map>

namespace Engine {
	class JobController : public Base {
	public:
		explicit JobController(const JobControllerDesc& desc);
		~JobController();

		void runTick(const std::vector<TickedSystem*>& systems, d64 dt);

		// Manual assignment of conflicting interactions between systems.
		// Most are checked using masks but some have external resources that are covered like this.
		// after must be in a strictly later batch than before
		// This is currently used to prevent the collision event buffer from causing race conditions
		void addOrderingConstraint(TickedSystem* before, TickedSystem* after); 
	private:
		bool conflicts(const Job& a, const Job& b) const;

		ThreadPool& m_threadPool;
		std::vector<std::pair<TickedSystem*, TickedSystem*>> m_orderingConstraints;

		// DEBUG REMOVE BELOW FIELDS
		d64 m_tickTimeAccum{ 0.0 };
		i32 m_tickCount{ 0 };
	};
}