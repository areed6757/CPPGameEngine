#pragma once
#include <Core/Common.h>

namespace Engine {
	class Scheduler : public Base {
	public:
		explicit Scheduler(const SchedulerDesc& desc);
		~Scheduler();

		Scheduler(const Scheduler&) = delete;
		Scheduler& operator = (const Scheduler&) = delete;



	private:
		GameClock& m_clock;
	};
}