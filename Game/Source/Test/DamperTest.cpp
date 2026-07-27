// DamperTest.cpp
#include <Test/DamperTest.h>
#include <Components/Position.h>
#include <Components/Movement.h>
#include <Components/Physics.h>
#include <Components/Renderable.h>
#include <Components/Thruster.h>
#include <Components/MovementDamper.h>
#include <Graphics/MeshID.h>

namespace Engine {
	DamperTest::DamperTest(const DamperTestDesc& desc) : Base(desc.base), m_ecs(desc.ecs) {}
	DamperTest::~DamperTest() {}

	void DamperTest::spawnComparisonRow(d64 laneSpacing, f32 initialSpeed,
		f32 linearDamping, f32 angularDamping)
	{
		auto spawnBase = [&](d64 lane) -> EntityID {
			EntityID id = m_ecs.createEntity();
			m_ecs.addComponent(id, Position{ .transform = Vector2double{ 0.0, lane }, .rotation = 0.0f });
			m_ecs.addComponent(id, Movement{
				.linearVelocity = Vector2float{ initialSpeed, 0.0f },
				.angularVelocity = 1.0f // nonzero so angular damping is also visible/comparable
				});
			m_ecs.addComponent(id, Renderable{ .mesh = MeshID::Quad, .texture = std::nullopt, .scale = 1.0f });
			m_ecs.addComponent(id, Physics{ .radius = 0.5f });
			m_ecs.addComponent(id, MovementDamper{ .linearDamping = linearDamping, .angularDamping = angularDamping });
			return id;
			};

		// Lane 1: no Thruster at all -- damper should always apply.
		spawnBase(-laneSpacing);

		// Lane 2: Thruster present but idle (throttle 0) -- damper should
		// still apply, since throttle is below the DamperSystem threshold.
		EntityID idle = spawnBase(0.0);
		m_ecs.addComponent(idle, Thruster{ .maxAccel = 5.0f, .throttle = 0.0f });

		// Lane 3: Thruster actively firing -- damper should NOT apply, this
		// entity's velocity should keep climbing from thrust instead of
		// decaying, despite starting at the same initial speed as the others.
		EntityID thrusting = spawnBase(laneSpacing);
		m_ecs.addComponent(thrusting, Thruster{ .maxAccel = 5.0f, .throttle = 1.0f });

		EngineLogInfo("DamperTest: spawned 3-lane comparison (no-thruster, idle-thruster, active-thruster), initial speed {}, damping ({}, {})",
			initialSpeed, linearDamping, angularDamping);
	}
}