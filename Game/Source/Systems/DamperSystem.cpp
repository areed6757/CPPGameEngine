#include <Systems/DamperSystem.h>

namespace Engine {
	DamperSystem::DamperSystem(const DamperSystemDesc& desc) : Base(desc.base), m_ecs(desc.ecs) 
	{
		m_entityMask = m_ecs.makeSignature<Movement, Thruster, MovementDamper>();
		m_reads = m_ecs.makeSignature<MovementDamper, Thruster>();
		m_writes = m_ecs.makeSignature<Movement>();
	}

	DamperSystem::~DamperSystem() {}

	void DamperSystem::Update(d64 dt)
	{
		i32 c = m_ecs.sizeComponentPool<MovementDamper>();
		for (i32 i = 0; i < c; i++) {
			i32 entityIndex = m_ecs.entityAtDenseIndex<MovementDamper>(i);
			EntityID id = m_ecs.entityFromIndex(entityIndex);
			if ((m_ecs.getSignature(id) & m_entityMask) != m_entityMask) { continue; }

			if (m_ecs.hasComponent<Thruster>(id)) {
				auto& thruster = m_ecs.getComponent<Thruster>(id);
				if (std::abs(thruster.throttle) > 0.01f) { continue; }
			}
			
			auto& damper = m_ecs.getComponentAtDenseIndex<MovementDamper>(i);
			auto& movement = m_ecs.getComponent<Movement>(id);

			f32 fdt = static_cast<f32>(dt);
			f32 linearFactor = std::max(0.0f, 1.0f - damper.linearDamping * fdt);
			f32 angularFactor = std::max(0.0f, 1.0f - damper.angularDamping * fdt);

			movement.linearVelocity = movement.linearVelocity * linearFactor;
			movement.angularVelocity *= angularFactor;

			f32 speed = std::sqrt(movement.linearVelocity.x * movement.linearVelocity.x + movement.linearVelocity.y * movement.linearVelocity.y);
			if (speed < damper.linearStopThreshold) { movement.linearVelocity = Vector2float{0.0f, 0.0f}; }
			if (std::abs(movement.angularVelocity) < damper.angularStopThreshold) { movement.angularVelocity = 0.0f; }
		}
	}

}