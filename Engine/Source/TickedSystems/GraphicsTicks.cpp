#include <TickedSystems/GraphicsTicks.h>
#include <format>

Engine::GraphicsTicks::GraphicsTicks(GraphicsTicksDesc& desc) : Base(desc.base), m_tickCount{0}
{
	EngineLogInfo("Graphics ticked system created.");
}

Engine::GraphicsTicks::~GraphicsTicks()
{
	EngineLogInfo("Graphics ticked system destroyed.")
}

void Engine::GraphicsTicks::Update(d64 dTime)
{
	// EngineLogInfo(std::format("Tick {} : dt: {}", m_tickCount++, dTime).c_str()); // Test logging
}
