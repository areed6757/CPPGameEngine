#include <Utilities/GameClock.h>

Engine::GameClock::GameClock(const GameClockDesc& desc) : Base(desc.base)
{
	EngineLogInfo("GameClock created.");
}

Engine::GameClock::~GameClock()
{
	EngineLogInfo("GameClock destroyed.");
}

void Engine::GameClock::tick() noexcept {
	auto now = SteadyClock::now();
	m_delta = std::chrono::duration_cast<Duration>(now - m_last).count();
	m_last = now;
}
