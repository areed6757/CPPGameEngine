#include <Systems/ImpulseSystem.h>
#include <Components/Movement.h>

namespace Engine {
	ImpulseSystem::ImpulseSystem(const ImpulseSystemDesc& desc) : Base(desc.base),
		m_ecs(desc.ecs),
		m_collisionSystem(desc.collisionSystem)
	{
	
	}

	ImpulseSystem::~ImpulseSystem() {

	}

	void ImpulseSystem::Update(d64 dt) {
		for (auto& event : m_collisionSystem.getEvents()) {
			if (m_ecs.hasComponent<Movement>(event.entityA))
				m_ecs.getComponent<Movement>(event.entityA).linearVelocity += event.impulseA;
			if (m_ecs.hasComponent<Movement>(event.entityB))
				m_ecs.getComponent<Movement>(event.entityB).linearVelocity += event.impulseB;
		}
	}
}