#include <Systems/MountFollowSystem.h>

namespace Engine {
	MountFollowSystem::MountFollowSystem(const MountFollowSystemDesc& desc) : Base(desc.base), m_ecs(desc.ecs)
	{
		m_entityMask = m_ecs.makeSignature<Position, Mount>();
		m_reads = m_ecs.makeSignature<Position, Mount, Weapon>();
		m_writes = m_ecs.makeSignature<Position>();
	}

	MountFollowSystem::~MountFollowSystem()
	{

	}

	void MountFollowSystem::Update(d64 dt)
	{
		i32 c = m_ecs.sizeComponentPool<Mount>();
		for (i32 i = 0; i < c; i++) {
			i32 entityIndex = m_ecs.entityAtDenseIndex<Mount>(i);
			EntityID id = m_ecs.entityFromIndex(entityIndex);
			if ((m_ecs.getSignature(id) & m_entityMask) != m_entityMask) { continue; }

			auto& mount = m_ecs.getComponent<Mount>(id);
			if (!m_ecs.isValidEntity(mount.owner)) { continue; } // Weapon system handles destroy

			auto& ownerPos = m_ecs.getComponent<Position>(mount.owner);
			auto& myPos = m_ecs.getComponent<Position>(id);

			f32 shipRot = ownerPos.rotation;
			Vector2float facing{ std::cos(shipRot), std::sin(shipRot) };
			Vector2double worldOffset{
				mount.offset.x * facing.x - mount.offset.y * facing.y,
				mount.offset.x * facing.y + mount.offset.y * facing.x
			};

			// The mount's world position tracks only the hull's rotation (it's fixed to the
			// anchor point); the weapon sprite drawn at that position swings independently on top.
			myPos.transform = ownerPos.transform + worldOffset;
			myPos.rotation = shipRot;
			if (m_ecs.hasComponent<Weapon>(id)) {
				myPos.rotation += m_ecs.getComponent<Weapon>(id).aimRotation;
			}
		}
	}
}