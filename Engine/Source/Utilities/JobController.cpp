#include <Utilities/JobController.h>

namespace Engine {
	JobController::JobController(const JobControllerDesc& desc) : Base(desc.base),
		m_threadPool(desc.threadPool)
	{
		EngineLogInfo("Job controller created.");
	}

	JobController::~JobController()
	{
		EngineLogInfo("Job controller destroyed.");
	}

	void JobController::runTick(const std::vector<TickedSystem*>& systems, d64 dt)
	{
		auto tickStart = std::chrono::high_resolution_clock::now(); // DEBUG REMOVE

		std::vector<Job> allJobs;
		for (auto* sys : systems) {
			auto jobs = sys->buildJobs(dt);
			allJobs.insert(allJobs.end(),
				std::make_move_iterator(jobs.begin()),
				std::make_move_iterator(jobs.end()));
		}

		std::unordered_map<TickedSystem*, i32> maxBatchPlaced;

		std::vector<std::vector<Job>> batches;
		for (auto& job : allJobs) {
			// Compute the earliest batch index this job is allowed to join
			// given every constraint where this job's owner is the after side
			i32 minAllowedBatch = 0;
			for (auto& [before, after] : m_orderingConstraints) {
				if (after != job.owner) { continue; }
				auto it = maxBatchPlaced.find(before);
				i32 beforeMax = (it != maxBatchPlaced.end()) ? it->second : -1;
				minAllowedBatch = std::max(minAllowedBatch, beforeMax + 1);
			}

			bool placed = false;
			for (i32 b = minAllowedBatch; b < static_cast<i32>(batches.size()); b++) {
				bool conflictsWithBatch = false;
				for (const auto& existing : batches[b]) {
					if (conflicts(job, existing)) { conflictsWithBatch = true; break; }
				}
				if (!conflictsWithBatch) {
					i32 placedIndex = b;
					batches[b].push_back(std::move(job));
					maxBatchPlaced[batches[b].back().owner] = std::max(maxBatchPlaced[batches[b].back().owner], placedIndex);
					placed = true;
					break;
				}
			}
			if (!placed) {
				i32 placedIndex = static_cast<i32>(batches.size());
				TickedSystem* owner = job.owner;
				batches.push_back({ std::move(job) });
				maxBatchPlaced[owner] = std::max(maxBatchPlaced[owner], placedIndex);
			}
		}

		for (auto& batch : batches) {
			for (auto& job : batch) {
				m_threadPool.submit(job.execute);
			}
			m_threadPool.waitForAll();
		}

		// DEBUG REMOVE THE REST OF THIS FUNCTION

		auto tickEnd = std::chrono::high_resolution_clock::now();
		auto ms = std::chrono::duration<d64, std::milli>(tickEnd - tickStart).count();
		m_tickTimeAccum += ms;
		m_tickCount++;
		if (m_tickCount >= 60) {
			EngineLogInfo("JobController: avg tick job time over last {} ticks: {:.3f}ms", m_tickCount, m_tickTimeAccum/ m_tickCount);
			m_tickTimeAccum = 0.0;
			m_tickCount = 0;
		}
	}

	bool JobController::conflicts(const Job& a, const Job& b) const
	{
		return (a.writes & (b.reads | b.writes)).any() ||
			(b.writes & (a.reads | a.writes)).any();
	}

	void JobController::addOrderingConstraint(TickedSystem* before, TickedSystem* after) {
		m_orderingConstraints.emplace_back(before, after);
	}
}