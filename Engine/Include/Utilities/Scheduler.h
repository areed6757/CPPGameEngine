#pragma once
#include <Core/Common.h>
#include <Utilities/GameClock.h>
#include <Utilities/CommandBuffer.h>
#include <Utilities/JobController.h>
#include <functional>
#include <vector>

namespace Engine {
	struct SchedulerDesc {
		BaseDesc base;
		GameClock& gameClock;
		JobController& jobController;
		d64 tickRate{ TICK_RATE };
	};

	class Scheduler : public Base {
	public:
		explicit Scheduler(const SchedulerDesc& desc);
		~Scheduler();

		Scheduler(const Scheduler&) = delete;
		Scheduler& operator = (const Scheduler&) = delete;

		void registerSystem(TickedSystem* sys);
		void registerFrameSystem(TickedSystem* sys);

		void advance();

		void togglePause();

		void registerFlushCallback(std::function<void()> cb);

		// how far into the next fixed tick the sim currently is, 0 to 1, for frame systems to smooth rendering against
		[[nodiscard]] f32 getInterpolationAlpha() const noexcept { return static_cast<f32>(m_accumulator / m_fixedTimestep); }

	private:
		GameClock& m_clock;
		d64 m_fixedTimestep;
		std::vector<TickedSystem*> m_systems{};
		std::vector<TickedSystem*> m_frameSystems{};
		d64 m_accumulator;
		bool m_paused{ false };
		std::vector<std::function<void()>> m_flushCallbacks{};
		JobController& m_jobController;

	};
}