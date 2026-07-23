#include <Systems/LifetimeSystem.h>
#include <Components/Lifetime.h>

namespace Engine {
	LifetimeSystem::LifetimeSystem(const LifetimeSystemDesc& desc) : Base(desc.base),
		m_ecs(desc.ecs),
		m_cmdBuffer({desc.base, desc.ecs})
	{
		m_entityMask = m_ecs.makeSignature<Lifetime>();
		m_reads = m_ecs.makeSignature<Lifetime>();
		m_writes = m_ecs.makeSignature<Lifetime>();
	}

	LifetimeSystem::~LifetimeSystem()
	{
	}

	void LifetimeSystem::Update(d64 dt)
	{
		for (i32 i = 0; i < m_ecs.sizeComponentPool<Lifetime>(); i++) {
			i32 entityIndex = m_ecs.entityAtDenseIndex<Lifetime>(i);
			EntityID id = m_ecs.entityFromIndex(entityIndex);

			auto& lifetime = m_ecs.getComponentAtDenseIndex<Lifetime>(i);

			lifetime.remaining -= static_cast<f32>(dt);

			if (lifetime.remaining <= 0.0f) {
				m_ecs.destroyEntity(id);
				i--; // Swap and pop from destroy will reindex the final entry of the dense array to this index
			}
		}
	}
}