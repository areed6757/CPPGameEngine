#pragma once
#include <Core/Common.h>
#include <Utilities/Job.h>
#include <bitset>

// Base class for all systems that are updated per-tick.
// DeltaTime is used to determine actual changes made per tick-call to Update()
namespace Engine {
	class TickedSystem {
	public:
		virtual void Update(d64 deltaTime) = 0;
		virtual ~TickedSystem() = default;

		std::bitset<64> getReadSignature() const noexcept { return m_reads; }
		std::bitset<64> getWriteSignature() const noexcept { return m_writes; }

		/// <summary>
		/// This is overriden by systems that use job batching instead of only relying on single-threaded Update().
		/// Default is one call to Update(dt) so all systems already use this and require no code changes, but threading
		/// can be added when useful later.
		/// </summary>
		/// <param name="dt"></param>
		/// <returns></returns>
		virtual std::vector<Job> buildJobs(d64 dt) {
			return { Job{m_reads, m_writes, [this, dt]() { Update(dt); } } };
		}

	protected:
		std::bitset<64> m_reads{};
		std::bitset<64> m_writes{};
	};
}