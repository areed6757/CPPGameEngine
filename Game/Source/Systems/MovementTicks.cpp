#include <Systems/MovementTicks.h>
#include <bitset>

namespace Engine {
	MovementTicks::MovementTicks(const MovementTicksDesc& desc) : Base(desc.base),
		m_ecs(desc.ecs),
		m_collisionSystem(desc.collisionSystem)
	{
		m_entityMask = m_ecs.makeSignature<Position, Movement>(); // Defines entities that can be modified by this system
		m_reads = m_ecs.makeSignature<Position, Movement>();
		m_writes = m_ecs.makeSignature<Position, Movement>();
	}

	MovementTicks::~MovementTicks()
	{
	}

	void MovementTicks::drainImpulses() {
		for (auto& event : m_collisionSystem.getEvents()) {
			if (m_ecs.hasComponent<Movement>(event.entityA))
				m_ecs.getComponent<Movement>(event.entityA).linearVelocity += event.impulseA;
			if (m_ecs.hasComponent<Movement>(event.entityB))
				m_ecs.getComponent<Movement>(event.entityB).linearVelocity += event.impulseB;
		}
	}

	void MovementTicks::updateRange(i32 start, i32 end, d64 dt) {
		for (i32 i = start; i < end; i++) {
			i32 entityIndex = m_ecs.entityAtDenseIndex<Movement>(i);
			EntityID id = m_ecs.entityFromIndex(entityIndex);
			if ((m_ecs.getSignature(id) & m_entityMask) != m_entityMask) { continue; }

			auto& tform = m_ecs.getComponent<Position>(id);
			auto& movement = m_ecs.getComponentAtDenseIndex<Movement>(i);

			f32 fdt = static_cast<f32>(dt);
			movement.linearVelocity += movement.linearAcceleration * fdt;
			movement.angularVelocity += movement.angularAcceleration * fdt;
			tform.transform += Vector2double(movement.linearVelocity * fdt);
			tform.rotation += movement.angularVelocity * fdt;
		}
	}

	void MovementTicks::Update(d64 dt)
	{
		drainImpulses();
		updateRange(0, m_ecs.sizeComponentPool<Movement>(), dt);

		// Debug
		// EngineLogInfo("Entity: {} moved to position: x {} y {}, velocity: ({}, {})", id.id, tform.transform.x, tform.transform.y, movement.linearVelocity);
	}

	std::vector<Job> MovementTicks::buildJobs(d64 dt) {
		drainImpulses(); // synchronous, before any chunk job is constructed

		i32 total = m_ecs.sizeComponentPool<Movement>();
		i32 chunkCount = 8; // To be tuned/determined automatically later
		i32 chunkSize = (total + chunkCount - 1) / chunkCount;

		std::vector<Job> jobs;
		for (i32 c = 0; c < chunkCount; c++) {
			i32 start = c * chunkSize;
			i32 end = std::min(start + chunkSize, total);
			if (start >= end) { break; }
			jobs.push_back(Job{ m_reads, m_writes, [this, start, end, dt]() {
				updateRange(start, end, dt);
			} });
		}
		return jobs;
	}
}