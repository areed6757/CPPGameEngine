#include <bitset>
#include <ECS/Systems/MovementTicks.h>

Engine::MovementTicks::MovementTicks(const MovementTicksDesc& desc) : Base(desc.base), m_ecs(desc.ecs)
{
	m_entityMask = m_ecs.makeSignature<Transform, Movement>();
}

Engine::MovementTicks::~MovementTicks()
{
}

void Engine::MovementTicks::Update(d64 dt)
{
	i32 c = m_ecs.sizeComponentPool<Movement>();

	for (i32 i = 0; i < c; i++) {
		i32 entityIndex = m_ecs.entityAtDenseIndex<Movement>(i);
		EntityID id = m_ecs.entityFromIndex(entityIndex);

		if ((m_ecs.getSignature(id) & m_entityMask) != m_entityMask) { continue; }

		auto& tform = m_ecs.getComponent<Transform>(id);
		auto& movement = m_ecs.getComponent<Movement>(id);

		movement.linearVelocity += movement.linearAcceleration * dt;
		movement.angularVelocity += movement.angularAcceleration * dt;

		f32 dx = std::cos(tform.rotation) * movement.linearVelocity;
		f32 dy = std::sin(tform.rotation) * movement.linearVelocity;

		tform.transform.x += static_cast<d64>(dx) * dt;
		tform.transform.y += static_cast<d64>(dy) * dt;

		tform.rotation += movement.angularVelocity * dt;

		// EngineLogInfo(std::format("Entity: {} moved to position: x {} y {}, velocity: {}", id.id, tform.transform.x, tform.transform.y, movement.linearVelocity).c_str());
	}
}