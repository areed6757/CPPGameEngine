#include <Utilities/Scheduler.h>
#include <ECS/TickedSystem.h>
#include <format>

Engine::Scheduler::Scheduler(const SchedulerDesc& desc) : Base(desc.base),
m_clock(desc.gameClock), m_fixedTimestep(desc.tickRate), m_systems{}, m_accumulator{ 0.0 }, m_paused{ false }
{
	EngineLogInfo("Scheduler created.");
}

Engine::Scheduler::~Scheduler()
{
	EngineLogInfo("Scheduler destroyed.");
}

void Engine::Scheduler::registerSystem(TickedSystem* sys)
{
	m_systems.push_back(sys);
	EngineLogInfo(std::format("Scheduler registered TickedSystem type: {}", typeid(*sys).name()).c_str());
}

void Engine::Scheduler::registerFrameSystem(TickedSystem* sys) {
	m_frameSystems.push_back(sys);
	EngineLogInfo(std::format("Scheduler registered Frame-based TickedSystem type: {}", typeid(*sys).name()).c_str());
}

void Engine::Scheduler::advance()
{
	m_clock.tick();
	d64 dt = m_clock.getDelta();
	m_accumulator += dt;
	if (m_paused) { m_accumulator = 0; };

	while (m_accumulator >= m_fixedTimestep) {
		for (auto* sys : m_systems) {
			sys->Update(m_fixedTimestep);
		}
		m_accumulator -= m_fixedTimestep;
		// EngineLogInfo("______________Tick______________"); // Uncomment to view tick-delineated logging
	}

	for (auto* sys : m_frameSystems) {
		sys->Update(dt);
	}
}

void Engine::Scheduler::togglePause()
{
	if (m_paused) {
		EngineLogInfo("Game unpaused.");
		m_paused = false;
	}
	else {
		EngineLogInfo("Game paused.");
		m_paused = true;
	}
}
 