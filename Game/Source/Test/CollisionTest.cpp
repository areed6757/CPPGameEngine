#include <Test/CollisionTest.h>
#include <Components/Position.h>
#include <Components/Movement.h>
#include <Components/Renderable.h>
#include <Components/Physics.h>
#include <Graphics/MeshID.h>

namespace Engine {
	CollisionTest::CollisionTest(const CollisionTestDesc& desc) : Base(desc.base), m_ecs(desc.ecs) {}
	CollisionTest::~CollisionTest() {}

	void CollisionTest::spawnHeadOnPairs(i32 pairCount, d64 separation, f32 speed,
		f32 physicsRadius, f32 massA, f32 massB, f32 elasticity, d64 laneSpacing)
	{
		d64 halfLanes = (pairCount - 1) * laneSpacing / 2.0;

		for (i32 i = 0; i < pairCount; ++i) {
			d64 lane = i * laneSpacing - halfLanes;

			// A starts left, closes moving +x
			Vector2double posA{ -separation / 2.0, lane };
			EntityID a = m_ecs.createEntity();
			m_ecs.addComponent(a, Position{ .transform = posA, .rotation = 0.0f });
			m_ecs.addComponent(a, Movement{
				.linearVelocity = Vector2float{ speed, 0.0f },
				.angularVelocity = 0.0f,
				.linearAcceleration = Vector2float{},
				.angularAcceleration = 0.0f
				});
			m_ecs.addComponent(a, Renderable{
				.mesh = MeshID::Quad,
				.texture = std::nullopt,
				.scale = physicsRadius * 2.0f
				});
			m_ecs.addComponent(a, Physics{
				.radius = physicsRadius,
				.mass = massA,
				.elasticity = elasticity
				});

			// B starts right, closes moving -x
			Vector2double posB{ separation / 2.0, lane };
			EntityID b = m_ecs.createEntity();
			m_ecs.addComponent(b, Position{ .transform = posB, .rotation = 0.0f });
			m_ecs.addComponent(b, Movement{
				.linearVelocity = Vector2float{ -speed, 0.0f },
				.angularVelocity = 0.0f,
				.linearAcceleration = Vector2float{},
				.angularAcceleration = 0.0f
				});
			m_ecs.addComponent(b, Renderable{
				.mesh = MeshID::Quad,
				.texture = std::nullopt,
				.scale = physicsRadius * 2.0f
				});
			m_ecs.addComponent(b, Physics{
				.radius = physicsRadius,
				.mass = massB,
				.elasticity = elasticity
				});
		}

		EngineLogInfo("Spawned {} head-on collision pair(s), separation {}, speed {}, masses ({}, {}), elasticity {}",
			pairCount, separation, speed, massA, massB, elasticity);
	}
}