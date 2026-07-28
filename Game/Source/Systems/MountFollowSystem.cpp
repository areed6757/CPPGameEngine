#include <Systems/MountFollowSystem.h>

namespace Engine {
	MountFollowSystem::MountFollowSystem(const MountFollowSystemDesc& desc) : Base(desc.base), m_ecs(desc.ecs)
	{
		m_entityMask = m_ecs.makeSignature<Position, WeaponMount>();
		m_reads = m_ecs.makeSignature<Position, WeaponMount>();
		m_writes = m_ecs.makeSignature<Position>();
	}

	MountFollowSystem::~MountFollowSystem()
	{

	}

	void MountFollowSystem::Update(d64 dt)
	{
		i32 c = m_ecs.sizeComponentPool<WeaponMount>();
		for (i32 i = 0; i < c; i++) {
			i32 entityIndex = m_ecs.entityAtDenseIndex<WeaponMount>(i);
			EntityID id = m_ecs.entityFromIndex(entityIndex);
			if ((m_ecs.getSignature(id) & m_entityMask) != m_entityMask) { continue; }

			auto& mount = m_ecs.getComponent<WeaponMount>(id);
			if (!m_ecs.isValidEntity(mount.owner)) { continue; } // Weapon system handles destroy

			auto& ownerPos = m_ecs.getComponent<Position>(mount.owner);
			auto& myPos = m_ecs.getComponent<Position>(id);

			f32 rot = ownerPos.rotation;
			Vector2float facing{ std::cos(rot), std::sin(rot) };
			Vector2double worldOffset{
				mount.offset.x * facing.x - mount.offset.y * facing.y,
				mount.offset.x * facing.y + mount.offset.y * facing.x
			};

			myPos.transform = ownerPos.transform + worldOffset;
			myPos.rotation = rot;
		}
	}
}