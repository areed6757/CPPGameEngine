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

		void tick() noexcept;
		d64 getDelta() const noexcept { return m_delta; };
		
	private:
		SteadyClock::time_point m_last{SteadyClock::now()};
		double m_delta{ 0.0 };

	};
}