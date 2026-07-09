#include <Utilities/Scheduler.h>
#include <format>

Engine::Scheduler::Scheduler(const SchedulerDesc& desc) : Base(desc.base),
m_clock(desc.gameClock), m_fixedTimestep(desc.tickRate), m_systems{}, m_accumulator{0.0}
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

void Engine::Scheduler::advance()
{
	m_clock.tick();
	m_accumulator += m_clock.getDelta();
	while (m_accumulator >= m_fixedTimestep) {
		for (auto* sys : m_systems) {
			sys->Update(m_fixedTimestep);
		}
		m_accumulator -= m_fixedTimestep;
		// EngineLogInfo("______________Tick______________"); // Uncomment to view tick-delineated logging
	}
}
 