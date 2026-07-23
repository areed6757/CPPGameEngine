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
		m_cmdBuffer({desc.base, desc.ecs}),
		m_quadtree(desc.quadtree),
		m_maxRadiusSeenThisTick(0.0)
	{
		m_entityMask = m_ecs.makeSignature<Position, Physics>();
		m_movementMask = m_ecs.makeSignature<Movement>();
		m_nonPhysicsCollisionMask = m_ecs.makeSignature<DamagePayload>();

		m_reads = m_ecs.makeSignature<Position, Physics, Movement, DamagePayload>();
		m_writes = m_ecs.makeSignature<>();

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

	// Narrow phase sweep that uses projected position comparisons to prevent tunneling from high-velocity
	// + low-radius projectile hits
	bool CollisionSystem::narrowPhaseSwept(EntityID a, EntityID b, d64 dt) const
	{
		auto& posA = m_ecs.getComponent<Position>(a);
		auto& posB = m_ecs.getComponent<Position>(b);
		auto& physA = m_ecs.getComponent<Physics>(a);
		auto& physB = m_ecs.getComponent<Physics>(b);

		Vector2float velA{};
		Vector2float velB{};
		if ((m_ecs.getSignature(a) & m_movementMask) == m_movementMask) { velA = m_ecs.getComponent<Movement>(a).linearVelocity; }
		if ((m_ecs.getSignature(b) & m_movementMask) == m_movementMask) { velB = m_ecs.getComponent<Movement>(b).linearVelocity; }

		// Vector from A to B at t=0, and how that vector changes per unit time
		Vector2double delta0 = posB.transform - posA.transform;
		Vector2double relVel{
			static_cast<d64>(velB.x - velA.x),
			static_cast<d64>(velB.y - velA.y)
		};

		d64 radiusSum = static_cast<d64>(physA.radius) + static_cast<d64>(physB.radius);
		d64 relVelSq = relVel.x * relVel.x + relVel.y * relVel.y;

		// Neither entity closing on the other this tick (stationary relative to
		// each other) closest approach is just the current distance, t=0
		if (relVelSq < 1e-12) {
			d64 distSq = delta0.x * delta0.x + delta0.y * delta0.y;
			return distSq <= (radiusSum * radiusSum);
		}

		// Time of closest approach, unclamped, then clamped into this tick's
		// actual window t=0 is already covered as the lower clamp bound, so
		// this subsumes the static check rather than needing it run separately
		d64 t = -(delta0.x * relVel.x + delta0.y * relVel.y) / relVelSq;
		t = std::clamp(t, 0.0, dt);

		Vector2double closest{ delta0.x + relVel.x * t, delta0.y + relVel.y * t };
		d64 distSq = closest.x * closest.x + closest.y * closest.y;

		return distSq <= (radiusSum * radiusSum);
	}


	bool CollisionSystem::narrowPhaseShip(EntityID a, EntityID b, d64 dt) const {
		// Currently defaults to simple swept check until ship baking process is complete
		// baked ships will have grid casting based on the vector they are flattened to
		return narrowPhaseSwept(a, b, dt);
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
			if (narrowPhaseSwept(a, b, dt)) {
				CollisionEvent event{ a, b };
				if (!((m_ecs.getSignature(a) & m_nonPhysicsCollisionMask) == m_nonPhysicsCollisionMask) &&
					!((m_ecs.getSignature(b) & m_nonPhysicsCollisionMask) == m_nonPhysicsCollisionMask)) 
				{
					computeImpulse(a, b, event.impulseA, event.impulseB);
				}
				m_events.push_back(event);
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
		if (velAlongNormal > 0.0f) { return; } // returns if the entities are already separating

		f32 elasticity = std::min(physA.elasticity, physB.elasticity);
		f32 j = -(1.0f + elasticity) * velAlongNormal / (invMassA + invMassB);

		Vector2float impulse{ normal.x * j, normal.y * j };

		outImpulseA = Vector2float{ -impulse.x * invMassA, -impulse.y * invMassA };
		outImpulseB = Vector2float{ impulse.x * invMassB, impulse.y * invMassB };
	}
}