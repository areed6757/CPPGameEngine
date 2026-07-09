#include <Utilities/Scheduler.h>

Engine::Scheduler::Scheduler(const SchedulerDesc& desc) : Base(desc.base), m_clock(desc.gameClock)
{
	EngineLogInfo("Scheduler created.");
}

Engine::Scheduler::~Scheduler()
{
	EngineLogInfo("Scheduler destroyed.");
}
 