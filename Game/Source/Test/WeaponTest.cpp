#include <Test/WeaponTest.h>

namespace Engine {
	WeaponTest::WeaponTest(const WeaponTestDesc& desc) : Base(desc.base), m_ecs(desc.ecs) {}
	WeaponTest::~WeaponTest() {}

	void WeaponTest::spawnFiringShip(d64 targetDistance, f32 cooldown, f32 projectileSpeed,
		f32 projectileRadius, f32 projectileDamage, f32 targetHealth)
	{
		// Ship: stationary, facing +x, at origin. Physics/Renderable given so
		// it's a real visible, collidable entity a badly-aimed shot could
		// (and shouldn't) hit.
		EntityID ship = m_ecs.createEntity();
		m_ecs.addComponent(ship, Position{ .transform = Vector2double{ 0.0, 0.0 }, .rotation = 0.0f });
		m_ecs.addComponent(ship, Physics{ .radius = 1.0f, .mass = 5.0f });
		m_ecs.addComponent(ship, Renderable{ .mesh = MeshID::Quad, .texture = std::nullopt, .scale = 2.0f });

		// Weapon: its own entity, mounted 1.2 units forward of the ship's
		// center so the projectile doesn't spawn embedded in the ship's own
		// hull (the broad-phase source-skip protects against a same-firer
		// hit regardless, but starting the spawn point clear of the hull
		// makes narrow-phase behavior easier to reason about while testing).
		EntityID weaponEntity = m_ecs.createEntity();
		m_ecs.addComponent(weaponEntity, Mount{ .owner = ship, .offset = Vector2float{ 1.2f, 0.0f } });
		m_ecs.addComponent(weaponEntity, Weapon{
			.cooldown = cooldown,
			.timeSinceLastFire = 0.0f,
			.projectileSpeed = projectileSpeed,
			.projectileRadius = projectileRadius,
			.projectileDamage = projectileDamage
			});
		// Note: the weapon entity itself has no Position -- WeaponSystem
		// reads Position off mount.owner (the ship), not off the weapon
		// entity. Confirm this matches WeaponSystem's actual entityMask
		// before running -- if entityMask requires Position on the weapon
		// entity too, this spawn needs one added here.

		// Target: stationary downrange, directly in the ship's firing line.
		EntityID target = m_ecs.createEntity();
		m_ecs.addComponent(target, Position{ .transform = Vector2double{ targetDistance, 0.0 }, .rotation = 0.0f });
		m_ecs.addComponent(target, Physics{ .radius = 1.0f, .mass = 1.0f });
		m_ecs.addComponent(target, Renderable{ .mesh = MeshID::Quad, .texture = std::nullopt, .scale = 2.0f });
		m_ecs.addComponent(target, Health{ .current = targetHealth, .max = targetHealth });

		EngineLogInfo("WeaponTest: spawned firing ship, weapon (cooldown {}, damage {}), target (hp {}) at distance {}",
			cooldown, projectileDamage, targetHealth, targetDistance);
	}
}