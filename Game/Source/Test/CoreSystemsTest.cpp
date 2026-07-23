#include <Test/CoreSystemsTest.h>
#include <Components/Position.h>
#include <Components/Movement.h>
#include <Components/Physics.h>
#include <Components/Renderable.h>
#include <Components/Thruster.h>
#include <Components/Health.h>
#include <Components/DamagePayload.h>
#include <Graphics/MeshID.h>

namespace Engine {
	CoreSystemsTest::CoreSystemsTest(const CoreSystemsTestDesc& desc) : Base(desc.base), m_ecs(desc.ecs) {}
	CoreSystemsTest::~CoreSystemsTest() {}

	void CoreSystemsTest::spawnAll()
	{
		constexpr d64 ROW_SPACING = 30.0; // km between the three scenario rows
		constexpr f32 RADIUS = 0.5f;

		// --- Row 1 (y = -ROW_SPACING): ThrusterTest -- three entities, same
		// facing, increasing accel, all starting at rest at x = 0.
		{
			f32 accels[3] = { 1.0f, 3.0f, 6.0f };
			for (i32 i = 0; i < 3; ++i) {
				d64 lane = (i - 1) * 5.0; // spread along a sub-axis within the row
				EntityID id = m_ecs.createEntity();
				m_ecs.addComponent(id, Position{
					.transform = Vector2double{ 0.0, -ROW_SPACING + lane },
					.rotation = 0.0f // facing +x
					});
				m_ecs.addComponent(id, Movement{
					.linearVelocity = Vector2float{},
					.angularVelocity = 0.0f,
					.linearAcceleration = Vector2float{},
					.angularAcceleration = 0.0f
					});
				m_ecs.addComponent(id, Renderable{
					.mesh = MeshID::Quad,
					.texture = std::nullopt,
					.scale = RADIUS * 2.0f
					});
				m_ecs.addComponent(id, Physics{ .radius = RADIUS });
				m_ecs.addComponent(id, Thruster{ .maxAccel = accels[i], .throttle = 1.0f });
			}
			EngineLogInfo("CoreSystemsTest: spawned thruster row (3 entities)");
		}

		// --- Row 2 (y = 0): CollisionTest -- one head-on pair, unequal mass,
		// to visibly confirm impulse resolution.
		{
			d64 separation = 20.0;
			f32 speed = 5.0f;

			EntityID a = m_ecs.createEntity();
			m_ecs.addComponent(a, Position{ .transform = Vector2double{ -separation / 2.0, 0.0 }, .rotation = 0.0f });
			m_ecs.addComponent(a, Movement{ .linearVelocity = Vector2float{ speed, 0.0f } });
			m_ecs.addComponent(a, Renderable{ .mesh = MeshID::Quad, .texture = std::nullopt, .scale = RADIUS * 2.0f });
			m_ecs.addComponent(a, Physics{ .radius = RADIUS, .mass = 1.0f, .elasticity = 0.5f });

			EntityID b = m_ecs.createEntity();
			m_ecs.addComponent(b, Position{ .transform = Vector2double{ separation / 2.0, 0.0 }, .rotation = 0.0f });
			m_ecs.addComponent(b, Movement{ .linearVelocity = Vector2float{ -speed, 0.0f } });
			m_ecs.addComponent(b, Renderable{ .mesh = MeshID::Quad, .texture = std::nullopt, .scale = RADIUS * 2.0f });
			m_ecs.addComponent(b, Physics{ .radius = RADIUS, .mass = 4.0f, .elasticity = 0.5f }); // heavier -- deflects less

			EngineLogInfo("CoreSystemsTest: spawned collision pair (mass 1 vs 4)");
		}

		// --- Row 3 (y = +ROW_SPACING): DamageTest -- stationary target, one
		// projectile closing on it.
		{
			d64 distance = 20.0;
			f32 speed = 8.0f;
			f32 targetHealth = 50.0f;
			f32 payloadDamage = 25.0f; // wounds, doesn't kill -- easy to eyeball a partial-health result

			EntityID target = m_ecs.createEntity();
			m_ecs.addComponent(target, Position{ .transform = Vector2double{ 0.0, ROW_SPACING }, .rotation = 0.0f });
			m_ecs.addComponent(target, Renderable{ .mesh = MeshID::Quad, .texture = std::nullopt, .scale = RADIUS * 2.0f });
			m_ecs.addComponent(target, Physics{ .radius = RADIUS, .mass = 1.0f, .elasticity = 0.2f });
			m_ecs.addComponent(target, Health{ .current = targetHealth, .max = targetHealth });

			EntityID projectile = m_ecs.createEntity();
			m_ecs.addComponent(projectile, Position{ .transform = Vector2double{ -distance, ROW_SPACING }, .rotation = 0.0f });
			m_ecs.addComponent(projectile, Movement{ .linearVelocity = Vector2float{ speed, 0.0f } });
			m_ecs.addComponent(projectile, Renderable{ .mesh = MeshID::Quad, .texture = std::nullopt, .scale = RADIUS * 2.0f });
			m_ecs.addComponent(projectile, Physics{ .radius = RADIUS, .mass = 0.01f, .elasticity = 0.2f });
			m_ecs.addComponent(projectile, DamagePayload{ .amount = payloadDamage });

			EngineLogInfo("CoreSystemsTest: spawned damage pair (target hp {}, payload {})", targetHealth, payloadDamage);
		}
	}
}