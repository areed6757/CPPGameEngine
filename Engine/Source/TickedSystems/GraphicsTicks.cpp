#include <TickedSystems/GraphicsTicks.h>
#include <format>

Engine::GraphicsTicks::GraphicsTicks(GraphicsTicksDesc& desc) : Base(desc.base), m_tickCount{0}
{
}

Engine::GraphicsTicks::~GraphicsTicks()
{
}

void Engine::GraphicsTicks::Update(d64 dTime)
{
	EngineLogInfo(std::format("Tick {} : dt: {}", m_tickCount++, dTime).c_str());
}
