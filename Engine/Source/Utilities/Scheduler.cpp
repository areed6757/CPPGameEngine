#include <Utilities/Scheduler.h>
#include <ECS/TickedSystem.h>
#include <format>

namespace Engine {
	Scheduler::Scheduler(const SchedulerDesc& desc) : Base(desc.base),
		m_clock(desc.gameClock),
		m_fixedTimestep(desc.tickRate),
		m_systems{},
		m_accumulator{ 0.0 },
		m_paused{ false },
		m_jobController(desc.jobController)
	{
		EngineLogInfo("Scheduler created.");
	}

	Scheduler::~Scheduler()
	{
		EngineLogInfo("Scheduler destroyed.");
	}

	void Scheduler::registerSystem(TickedSystem* sys)
	{
		m_systems.push_back(sys);
		EngineLogInfo("Scheduler registered TickedSystem type: {}", typeid(*sys).name());
	}

	void Scheduler::registerFrameSystem(TickedSystem* sys) {
		m_frameSystems.push_back(sys);
		EngineLogInfo("Scheduler registered Frame-based TickedSystem type: {}", typeid(*sys).name());
	}

	void Scheduler::advance()
	{
		m_clock.tick();
		d64 dt = m_clock.getDelta();
		m_accumulator += dt;
		if (m_paused) { m_accumulator = 0; };

		while (m_accumulator >= m_fixedTimestep) {
			m_jobController.runTick(m_systems, m_fixedTimestep);
			for (auto& cb : m_flushCallbacks) { cb(); }
			m_accumulator -= m_fixedTimestep;
		}
		for (auto* sys : m_frameSystems) {
			sys->Update(dt);
		}
	}

	void Scheduler::togglePause()
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

	void Scheduler::registerFlushCallback(std::function<void()> cb) {
		m_flushCallbacks.push_back(std::move(cb));
		EngineLogInfo("Scheduler registered a command buffer flush callback.");
	}
}