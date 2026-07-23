#include <Test/DamageTest.h>
#include <Components/Position.h>
#include <Components/Movement.h>
#include <Components/Physics.h>
#include <Components/Renderable.h>
#include <Components/Health.h>
#include <Components/DamagePayload.h>
#include <Graphics/MeshID.h>

namespace Engine {
	DamageTest::DamageTest(const DamageTestDesc& desc) : Base(desc.base), m_ecs(desc.ecs) {}
	DamageTest::~DamageTest() {}

	void DamageTest::spawnTestHit(d64 distance, f32 speed, f32 physicsRadius,
		f32 targetHealth, f32 payloadDamage)
	{
		// Target: stationary, at origin, no Movement/Thruster -- pure Health test.
		EntityID target = m_ecs.createEntity();
		m_ecs.addComponent(target, Position{
			.transform = Vector2double{ 0.0, 0.0 },
			.rotation = 0.0f
			});
		m_ecs.addComponent(target, Renderable{
			.mesh = MeshID::Quad,
			.texture = std::nullopt,
			.scale = physicsRadius * 2.0f
			});
		m_ecs.addComponent(target, Physics{
			.radius = physicsRadius,
			.mass = 1.0f,
			.elasticity = 0.2f
			});
		m_ecs.addComponent(target, Health{
			.current = targetHealth,
			.max = targetHealth
			});

		// Projectile: starts `distance` to the left, moving right toward target.
		EntityID projectile = m_ecs.createEntity();
		m_ecs.addComponent(projectile, Position{
			.transform = Vector2double{ -distance, 0.0 },
			.rotation = 0.0f
			});
		m_ecs.addComponent(projectile, Movement{
			.linearVelocity = Vector2float{ speed, 0.0f },
			.angularVelocity = 0.0f,
			.linearAcceleration = Vector2float{},
			.angularAcceleration = 0.0f
			});
		m_ecs.addComponent(projectile, Renderable{
			.mesh = MeshID::Quad,
			.texture = std::nullopt,
			.scale = physicsRadius * 2.0f
			});
		m_ecs.addComponent(projectile, Physics{
			.radius = physicsRadius,
			.mass = 0.01f, // deliberately light -- if impulse skip ever regresses, a light-vs-heavy pair makes any bounce-back obvious
			.elasticity = 0.2f
			});
		m_ecs.addComponent(projectile, DamagePayload{
			.amount = payloadDamage
			});

		EngineLogInfo("Spawned damage test: target health {}, projectile payload {}, closing at speed {}",
			targetHealth, payloadDamage, speed);
	}
}