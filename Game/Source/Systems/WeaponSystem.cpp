#include <Systems/WeaponSystem.h>

namespace Engine {
	WeaponSystem::WeaponSystem(const WeaponSystemDesc& desc) : Base(desc.base),
		m_ecs(desc.ecs),
		m_cmdBuffer({desc.base, desc.ecs})
	{
		m_entityMask = m_ecs.makeSignature<Position, Weapon>();
		m_reads = m_ecs.makeSignature<Position, Weapon>();
		m_writes = m_ecs.makeSignature<Weapon>();
	}

}