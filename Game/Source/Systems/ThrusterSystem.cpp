#include <Systems/ThrusterSystem.h>
#include <Components/Thruster.h>
#include <Components/Movement.h>
#include <Components/Position.h>
#include <Physics/Vector2float.h>
#include <cmath>


namespace Engine {
	ThrusterSystem::ThrusterSystem(const ThrusterSystemDesc& desc) : Base(desc.base),
		m_ecs(desc.ecs),
		m_cmdBuffer({desc.base, desc.ecs})
	{
		m_entityMask = m_ecs.makeSignature<Position, Movement, Thruster>();
		m_reads = m_ecs.makeSignature<Position, Thruster>();
		m_writes = m_ecs.makeSignature<Movement>();
	}

	ThrusterSystem::~ThrusterSystem()
	{
	}

	void ThrusterSystem::Update(d64 dt)
	{
		for (i32 i = 0; i < m_ecs.sizeComponentPool<Thruster>(); i++) {
			i32 entityIndex = m_ecs.entityAtDenseIndex<Thruster>(i);
			EntityID id = m_ecs.entityFromIndex(entityIndex);
			if ((m_ecs.getSignature(id) & m_entityMask) != m_entityMask) { continue; }

			auto& pos = m_ecs.getComponent<Position>(id);
			auto& movement = m_ecs.getComponent<Movement>(id);
			auto& thruster = m_ecs.getComponentAtDenseIndex<Thruster>(i);

			f32 rot = pos.rotation;

			Vector2float facing{ std::cos(rot), std::sin(rot) };

			f32 accel = thruster.maxAccel * thruster.throttle;

			movement.linearAcceleration = { facing.x * accel, facing.y * accel };
		}
	}

}