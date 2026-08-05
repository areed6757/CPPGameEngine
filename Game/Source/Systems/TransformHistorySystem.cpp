#include <Systems/TransformHistorySystem.h>

namespace Engine {
	TransformHistorySystem::TransformHistorySystem(const TransformHistorySystemDesc& desc) : Base(desc.base),
		m_ecs(desc.ecs)
	{
		m_reads = m_ecs.makeSignature<Position>();
		m_writes = m_ecs.makeSignature<Position>();
	}

	TransformHistorySystem::~TransformHistorySystem()
	{
	}

	void TransformHistorySystem::Update(d64 dt)
	{
		i32 c = m_ecs.sizeComponentPool<Position>();
		for (i32 i = 0; i < c; i++) {
			auto& pos = m_ecs.getComponentAtDenseIndex<Position>(i);
			pos.prevTransform = pos.transform;
			pos.prevRotation = pos.rotation;
			pos.hasHistory = true;
		}
	}
}
