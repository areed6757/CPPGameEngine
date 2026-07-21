#include <Systems/CollisionSystem.h>
#include <Components/Physics.h>
#include <Components/Position.h>
#include <format>


namespace Engine {
	CollisionSystem::CollisionSystem(const CollisionSystemDesc& desc) : Base(desc.base),
		m_ecs(desc.ecs),
		m_quadtree(desc.quadtree)
	{
		m_entityMask = m_ecs.makeSignature<Position, Physics>();

		EngineLogInfo("Collision system created.");
	}

	CollisionSystem::~CollisionSystem()
	{
		EngineLogInfo("Collision system destroyed.");
	}

	void CollisionSystem::broadPhase(std::vector<CollisionCandidate>& outCandidates) {
		outCandidates.clear();
		
		i32 c = m_ecs.sizeComponentPool<Physics>();
		for (i32 i = 0; i < c; i++) {
			i32 entityIndex = m_ecs.entityAtDenseIndex<Physics>(i);
			EntityID id = m_ecs.entityFromIndex(entityIndex);
			if ((m_ecs.getSignature(id) & m_entityMask) != m_entityMask) { continue; }
		
			auto& tform = m_ecs.getComponent<Position>(id);
			auto& physics = m_ecs.getComponentAtDenseIndex<Physics>(i);

			m_nearbyScratch.clear();
			m_quadtree.queryRadius(tform.transform, static_cast<d64>(physics.radius) + m_maxRadiusSeenThisTick, m_nearbyScratch);

			for (EntityID other : m_nearbyScratch) {
				if (other.id == id.id) { continue; } // skips self
				if (other.id < id.id) { continue; } // pairs checked once

				outCandidates.push_back({ id, other });
			}
		}
	}

	bool CollisionSystem::narrowPhaseSimple(EntityID a, EntityID b) const {
		auto& posA = m_ecs.getComponent<Position>(a);
		auto& posB = m_ecs.getComponent<Position>(b);
		auto& physA = m_ecs.getComponent<Physics>(a);
		auto& physB = m_ecs.getComponent<Physics>(b);

		Vector2double delta = posA.transform - posB.transform;
		d64 distSq = delta.x * delta.x + delta.y * delta.y;
		d64 radiusSum = static_cast<d64>(physA.radius) + static_cast<d64>(physB.radius);

		return distSq <= (radiusSum * radiusSum);
	}


	bool CollisionSystem::narrowPhaseShip(EntityID a, EntityID b) const {
		// Currently defaults to simple circle check until ship baking process is complete
		// baked ships will have grid casting based on the vector they are flattened to
		return narrowPhaseSimple(a, b);
	}

	void CollisionSystem::Update(d64 dt)
	{
		m_quadtree.clear();
		m_maxRadiusSeenThisTick = 0.0; // Radius of the largest entity in shared quadtree cell
		i32 c = m_ecs.sizeComponentPool<Physics>();
		for (i32 i = 0; i < c; i++) {
			i32 entityIndex = m_ecs.entityAtDenseIndex<Physics>(i);
			EntityID id = m_ecs.entityFromIndex(entityIndex);
			if ((m_ecs.getSignature(id) & m_entityMask) != m_entityMask) { continue; }

			auto& tform = m_ecs.getComponent<Position>(id);
			m_quadtree.insert(id, tform.transform);

			auto& physics = m_ecs.getComponent<Physics>(id);
			m_maxRadiusSeenThisTick = std::max(m_maxRadiusSeenThisTick, static_cast<d64>(physics.radius));
		}

		broadPhase(m_candidates);

		m_events.clear();
		for (const auto& [a, b] : m_candidates) {
			// TODO: dispatch narrowPhaseSimple vs narrowPhaseShip once
			// Currently all collisions are simple non-ship
			// can differentiate the two by the presense of segment data on baked ships
			if (narrowPhaseSimple(a, b)) {
				m_events.push_back(CollisionEvent{ a, b });
				EngineLogInfo("Collision detected between entity: {} and entity: {}", a.id, b.id);
				// TODO: compute the impulses on both ships based on their physics
			}
		}
	}

	// Exposes resolved collision data for consumer systems
	const std::vector<CollisionEvent>& CollisionSystem::getEvents() const noexcept {
		return m_events;
	}
}