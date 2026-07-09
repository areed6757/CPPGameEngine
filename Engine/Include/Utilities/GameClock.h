#pragma once
#include <Core/Common.h>
#include <chrono>

namespace Engine {
	using SteadyClock = std::chrono::steady_clock;
	using Duration = std::chrono::duration<double>;

	class GameClock : public Base {
	public:
		explicit GameClock(const GameClockDesc& desc);
		~GameClock();

		void tick() noexcept {
			auto now = SteadyClock::now();
			m_delta = std::chrono::duration_cast<Duration>(now - m_last).count();
			m_last = now;
		}
		
	private:
		SteadyClock::time_point m_last{SteadyClock::now()};
		double m_delta{ 0.0 };

	};
}