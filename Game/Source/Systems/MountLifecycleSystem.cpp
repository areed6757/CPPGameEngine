#include <Systems/MountLifecycleSystem.h>
#include <Components/Mount.h>

namespace Engine {
	MountLifecycleSystem::MountLifecycleSystem(const MountLifecycleSystemDesc& desc) : Base(desc.base),
		m_ecs(desc.ecs),
		m_cmdBuffer({ desc.base, desc.ecs })
	{
		m_entityMask = m_ecs.makeSignature<Mount>();
		m_reads = m_ecs.makeSignature<Mount>();
		m_writes = m_ecs.makeSignature<>();
		EngineLogInfo("Mount lifecycle system created.");
	}

	MountLifecycleSystem::~MountLifecycleSystem()
	{
		EngineLogInfo("Mount lifecycle system destroyed.");
	}

	void MountLifecycleSystem::Update(d64 dt)
	{
		i32 c = m_ecs.sizeComponentPool<Mount>();
		for (i32 i = 0; i < c; i++) {
			i32 entityIndex = m_ecs.entityAtDenseIndex<Mount>(i);
			EntityID id = m_ecs.entityFromIndex(entityIndex);
			if ((m_ecs.getSignature(id) & m_entityMask) != m_entityMask) { continue; }

			auto& mount = m_ecs.getComponentAtDenseIndex<Mount>(i);
			if (!m_ecs.isValidEntity(mount.owner)) {
				m_cmdBuffer.destroyEntity(id);
			}
		}
	}
}