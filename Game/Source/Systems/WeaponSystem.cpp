#include <Systems/WeaponSystem.h>

namespace Engine {
	WeaponSystem::WeaponSystem(const WeaponSystemDesc& desc) : Base(desc.base),
		m_ecs(desc.ecs),
		m_cmdBuffer({desc.base, desc.ecs})
	{
		m_entityMask = m_ecs.makeSignature<Mount, Weapon>();
		m_reads = m_ecs.makeSignature<Mount, Weapon>();
		m_writes = m_ecs.makeSignature<Weapon>();
	}

	WeaponSystem::~WeaponSystem() {

	}

	void WeaponSystem::Update(d64 dt) 
	{
		i32 c = m_ecs.sizeComponentPool<Weapon>();
		for (i32 i = 0; i < c; i++) {
			i32 entityIndex = m_ecs.entityAtDenseIndex<Weapon>(i);
			EntityID id = m_ecs.entityFromIndex(entityIndex);
			if ((m_ecs.getSignature(id) & m_entityMask) != m_entityMask) { continue; }
			
			auto& mount = m_ecs.getComponent<Mount>(id);
			if (!m_ecs.isValidEntity(mount.owner)) {
				m_cmdBuffer.destroyEntity(id);
				continue;
			}

			auto& weapon = m_ecs.getComponentAtDenseIndex<Weapon>(i);
			weapon.timeSinceLastFire += static_cast<f32>(dt);
			if (weapon.timeSinceLastFire >= weapon.cooldown) { fire(id); }
		}
	}

	void WeaponSystem::fire(EntityID id) {
		auto& mount = m_ecs.getComponent<Mount>(id);
		auto& weapon = m_ecs.getComponent<Weapon>(id);
		auto& ownerPos = m_ecs.getComponent<Position>(mount.owner);

		f32 rot = ownerPos.rotation;
		Vector2float facing{ std::cos(rot), std::sin(rot) };

		Vector2double worldMountPos = ownerPos.transform + Vector2double{
			mount.offset.x * facing.x - mount.offset.y * facing.y,
			mount.offset.x * facing.y + mount.offset.y * facing.x
		};

		// TODO: These are hardcoded and need to be prefabs
		EntityID projectile = m_cmdBuffer.createEntity();
		m_cmdBuffer.addComponent(projectile, Position{ .transform = worldMountPos, .rotation = rot });
		m_cmdBuffer.addComponent(projectile, Movement{
			.linearVelocity = Vector2float{ facing.x * weapon.projectileSpeed, facing.y * weapon.projectileSpeed }
			});
		m_cmdBuffer.addComponent(projectile, Physics{ .radius = weapon.projectileRadius, .mass = 0.01f });
		m_cmdBuffer.addComponent(projectile, Lifetime{ .remaining = 3.0f });
		m_cmdBuffer.addComponent(projectile, DamagePayload{ .amount = weapon.projectileDamage, .source = mount.owner });
		m_cmdBuffer.addComponent(projectile, Renderable{ .mesh = MeshID::Quad, .texture = std::nullopt, .scale = weapon.projectileRadius * 2.0f });
		weapon.timeSinceLastFire = 0.0f;
	}
}