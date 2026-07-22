#include <Systems/CollisionSystem.h>
#include <Components/Physics.h>
#include <Components/Position.h>
#include <Components/DamagePayload.h>
#include <format>
#include <cmath>
#include <algorithm>

namespace Engine {
	CollisionSystem::CollisionSystem(const CollisionSystemDesc& desc) : Base(desc.base),
		m_ecs(desc.ecs),
		m_quadtree(desc.quadtree),
		m_maxRadiusSeenThisTick(0.0)
	{
		m_entityMask = m_ecs.makeSignature<Position, Physics>();
		m_movementMask = m_ecs.makeSignature<Movement>();
		m_nonPhysicsCollisionMask = m_ecs.makeSignature<DamagePayload>();

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
				CollisionEvent event{ a, b };
				if (!((m_ecs.getSignature(a) & m_nonPhysicsCollisionMask) == m_nonPhysicsCollisionMask) &&
					!((m_ecs.getSignature(b) & m_nonPhysicsCollisionMask) == m_nonPhysicsCollisionMask)) 
				{
					computeImpulse(a, b, event.impulseA, event.impulseB);
				}
				m_events.push_back(event);
				EngineLogInfo("Collision detected between entity: {} and entity: {}", a.id, b.id);
				// TODO: compute the impulses on both ships based on their physics
			}
		}
	}

	// Exposes resolved collision data for consumer systems
	const std::vector<CollisionEvent>& CollisionSystem::getEvents() const noexcept {
		return m_events;
	}

	// Do not divide result by mass again downstream, it is already a considered value of output
	// Output is simply a velocity delta
	void CollisionSystem::computeImpulse(EntityID a, EntityID b, Vector2float& outImpulseA, Vector2float& outImpulseB) const
	{
		outImpulseA = {};
		outImpulseB = {};

		auto& posA = m_ecs.getComponent<Position>(a);
		auto& posB = m_ecs.getComponent<Position>(b);
		auto& physA = m_ecs.getComponent<Physics>(a);
		auto& physB = m_ecs.getComponent<Physics>(b);

		Vector2double delta = posB.transform - posA.transform;
		f32 dist = std::sqrt(delta.x * delta.x + delta.y * delta.y);
		if (dist < 1e-6) { return; } // Stop proc for entities too close this tick to calculate accurately

		Vector2float normal{ static_cast<f32>(delta.x / dist), static_cast<f32>(delta.y / dist) };

		f32 invMassA = physA.mass > 0.0f ? 1.0f / physA.mass : 0.0f;
		f32 invMassB = physB.mass > 0.0f ? 1.0f / physB.mass : 0.0f;
		if (invMassA == 0.0f && invMassB == 0.0f) { return; }
		
		Vector2float velA{};
		Vector2float velB{};

		if ((m_ecs.getSignature(a) & m_movementMask) == m_movementMask) { velA = m_ecs.getComponent<Movement>(a).linearVelocity; }
		if ((m_ecs.getSignature(b) & m_movementMask) == m_movementMask) { velB = m_ecs.getComponent<Movement>(b).linearVelocity; }

		Vector2float relVel = velB - velA;
		f32 velAlongNormal = relVel.x * normal.x + relVel.y * normal.y;
		if (velAlongNormal > 0.0) { return; } // returns if the entities are already separating

		f32 elasticity = std::min(physA.elasticity, physB.elasticity);
		f32 j = -(1.0f + elasticity) * velAlongNormal / (invMassA + invMassB);

		Vector2float impulse{ normal.x * j, normal.y * j };

		outImpulseA = Vector2float{ -impulse.x * invMassA, -impulse.y * invMassA };
		outImpulseB = Vector2float{ impulse.x * invMassB, impulse.y * invMassB };
	}
}