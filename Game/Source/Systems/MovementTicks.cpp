#include <Systems/MovementTicks.h>
#include <bitset>

Engine::MovementTicks::MovementTicks(const MovementTicksDesc& desc) : Base(desc.base), m_ecs(desc.ecs)
{
	m_entityMask = m_ecs.makeSignature<Position, Movement>(); // Defines entities that can be modified by this system
}

Engine::MovementTicks::~MovementTicks()
{
}

void Engine::MovementTicks::Update(d64 dt)
{
	// Concrete selection of Movement Component pool size, as it will necessarily be smaller than Position
	for (i32 i = 0; i < m_ecs.sizeComponentPool<Movement>(); i++) {
		
		// Acquire components
		i32 entityIndex = m_ecs.entityAtDenseIndex<Movement>(i);
		EntityID id = m_ecs.entityFromIndex(entityIndex);
		if ((m_ecs.getSignature(id) & m_entityMask) != m_entityMask) { continue; } // Skips entities without both positon and movement
		
		// Movement is the dense array being parse, so entity dense index is known to be i
		// Position must still perform a lookup based on id
		auto& tform = m_ecs.getComponent<Position>(id);
		auto& movement = m_ecs.getComponentAtDenseIndex<Movement>(i);


		// Update components
		f32 fdt = static_cast<f32>(dt);
		movement.linearVelocity += movement.linearAcceleration * fdt;
		movement.angularVelocity += movement.angularAcceleration * fdt;

		tform.transform += Vector2double(movement.linearVelocity * fdt);
		tform.rotation += movement.angularVelocity * fdt;


		// Debug
		// EngineLogInfo(std::format("Entity: {} moved to position: x {} y {}, velocity: ({}, {})", id.id, tform.transform.x, tform.transform.y, movement.linearVelocity).c_str());
	}
}