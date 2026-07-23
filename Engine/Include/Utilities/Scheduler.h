#pragma once
#include <Core/Common.h>
#include <Utilities/GameClock.h>
#include <Utilities/CommandBuffer.h>
#include <functional>
#include <vector>

namespace Engine {
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

	private:
		GameClock& m_clock;
		d64 m_fixedTimestep;
		std::vector<TickedSystem*> m_systems{};
		std::vector<TickedSystem*> m_frameSystems{};
		d64 m_accumulator;
		bool m_paused{ false };
		std::vector<std::function<void()>> m_flushCallbacks{};

	};
}