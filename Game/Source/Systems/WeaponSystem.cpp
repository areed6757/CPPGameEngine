#include <Systems/WeaponSystem.h>
#include <Ships/IconLOD.h>
#include <random>

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

		f32 shipRot = ownerPos.rotation;
		Vector2float mountFacing{ std::cos(shipRot), std::sin(shipRot) };

		f32 aimRot = shipRot + weapon.aimRotation;
		Vector2float facing{ std::cos(aimRot), std::sin(aimRot) };

		static std::mt19937 rng{ std::random_device{}() };
		std::uniform_real_distribution<f32> spreadDist(-weapon.accuracy, weapon.accuracy);

		for (i32 b = 0; b < weapon.barrelCount; b++) {
			f32 shotRot = aimRot + spreadDist(rng);
			Vector2float shotFacing{ std::cos(shotRot), std::sin(shotRot) };

			Vector2float barrelLocal = weapon.barrelOffsets[b];
			Vector2double barrelWorldOffset{
				barrelLocal.x * facing.x - barrelLocal.y * facing.y,
				barrelLocal.x * facing.y + barrelLocal.y * facing.x
			};
			Vector2double worldMountPos = ownerPos.transform + Vector2double{
				mount.offset.x * mountFacing.x - mount.offset.y * mountFacing.y,
				mount.offset.x * mountFacing.y + mount.offset.y * mountFacing.x
			} + barrelWorldOffset;

			EntityID projectile = m_cmdBuffer.createEntity();
			m_cmdBuffer.addComponent(projectile, Position{ .transform = worldMountPos, .rotation = shotRot });
			m_cmdBuffer.addComponent(projectile, Movement{
				.linearVelocity = Vector2float{ shotFacing.x * weapon.projectileSpeed, shotFacing.y * weapon.projectileSpeed }
				});
			m_cmdBuffer.addComponent(projectile, Physics{ .radius = weapon.projectileRadius, .mass = 0.01f });
			m_cmdBuffer.addComponent(projectile, DamagePayload{ .amount = weapon.projectileDamage, .source = mount.owner });
			m_cmdBuffer.addComponent(projectile, Lifetime{ .remaining = weapon.projectileLifetime });
			m_cmdBuffer.addComponent(projectile, Renderable{
				.mesh = MeshID::Quad,
				.texture = std::nullopt,
				.scale = weapon.projectileRadius * 2.0f,
				.iconTexture = g_projectileIconTexture,
				.iconMinPixelSize = g_projectileIconMinPixelSize
				});
		}

		weapon.timeSinceLastFire = 0.0f;
	}
}