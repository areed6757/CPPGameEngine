#include <Test/ThrusterTest.h>
#include <Components/Position.h>
#include <Components/Movement.h>
#include <Components/Physics.h>
#include <Components/Renderable.h>
#include <Components/Thruster.h>
#include <Graphics/MeshID.h>

namespace Engine {
	ThrusterTest::ThrusterTest(const ThrusterTestDesc& desc) : Base(desc.base), m_ecs(desc.ecs) {}
	ThrusterTest::~ThrusterTest() {}

	void ThrusterTest::spawnAccelRow(i32 count, d64 laneSpacing, f32 minAccel, f32 maxAccel,
		f32 physicsRadius, f32 facingRotation)
	{
		d64 halfLanes = (count - 1) * laneSpacing / 2.0;

		for (i32 i = 0; i < count; ++i) {
			d64 lane = i * laneSpacing - halfLanes;

			f32 t = (count > 1) ? static_cast<f32>(i) / static_cast<f32>(count - 1) : 0.0f;
			f32 accel = minAccel + (maxAccel - minAccel) * t;

			Vector2double pos{ 0.0, lane };

			EntityID id = m_ecs.createEntity();
			m_ecs.addComponent(id, Position{
				.transform = pos,
				.rotation = facingRotation
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
				.scale = physicsRadius * 2.0f
				});
			m_ecs.addComponent(id, Physics{
				.radius = physicsRadius
				});
			m_ecs.addComponent(id, Thruster{
				.maxAccel = accel,
				.throttle = 1.0f
				});
			m_ecs.addComponent(id, Lifetime{
				.remaining = 3.0f
				});
		}

		EngineLogInfo("Spawned {} thrust-test entities, accel range {} to {}", count, minAccel, maxAccel);
	}
}