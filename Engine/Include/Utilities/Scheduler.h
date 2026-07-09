#pragma once
#include <Core/Common.h>
#include <Utilities/GameClock.h>

namespace Engine {
	class Scheduler : public Base {
	public:
		explicit Scheduler(const SchedulerDesc& desc);
		~Scheduler();

		Scheduler(const Scheduler&) = delete;
		Scheduler& operator = (const Scheduler&) = delete;

		void registerSystem(TickedSystem* sys);

		void advance();

		void togglePause();

	private:
		GameClock& m_clock;
		d64 m_fixedTimestep;
		std::vector<TickedSystem*> m_systems{};
		d64 m_accumulator;

		bool m_paused {false};
	};
}