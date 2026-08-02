#include <Systems/CollisionSystem.h>
#include <format>
#include <cmath>
#include <algorithm>

namespace Engine {
	CollisionSystem::CollisionSystem(const CollisionSystemDesc& desc) : Base(desc.base),
		m_ecs(desc.ecs),
		m_cmdBuffer({desc.base, desc.ecs}),
		m_aabbTree(desc.aabbTree),
		m_proxies(),
		m_threadPool(desc.threadPool)
	{
		m_entityMask = m_ecs.makeSignature<Position, Physics>();
		m_movementMask = m_ecs.makeSignature<Movement>();
		m_nonPhysicsCollisionMask = m_ecs.makeSignature<DamagePayload>();

		m_reads = m_ecs.makeSignature<Position, Physics, Movement, DamagePayload, Faction>();
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
		i32 threadCount = m_threadPool.threadCount();
		i32 chunkSize = (c + threadCount - 1) / threadCount;

		m_chunkCandidates.assign(threadCount, {});

		for (i32 t = 0; t < threadCount; t++) {
			i32 start = t * chunkSize;
			i32 end = std::min(start + chunkSize, c);
			if (start >= end) { continue; }

			m_threadPool.submit([this, start, end, t]() {
				auto& localCandidates = m_chunkCandidates[t];
				for (i32 i = start; i < end; i++) {
					i32 entityIndex = m_ecs.entityAtDenseIndex<Physics>(i);
					EntityID id = m_ecs.entityFromIndex(entityIndex);
					if ((m_ecs.getSignature(id) & m_entityMask) != m_entityMask) { continue; }

					auto& tform = m_ecs.getComponent<Position>(id);
					auto& physics = m_ecs.getComponentAtDenseIndex<Physics>(i);

					AABB queryBounds{
						Vector2double{ tform.transform.x - physics.radius, tform.transform.y - physics.radius },
						Vector2double{ tform.transform.x + physics.radius, tform.transform.y + physics.radius }
					};

					std::vector<EntityID> nearby; // thread-local
					m_aabbTree.query(queryBounds, nearby);

					for (EntityID other : nearby) {
						if (other.id == id.id) { continue; }
						if (other.id < id.id) { continue; }
						if (m_ecs.hasComponent<DamagePayload>(id) && m_ecs.getComponent<DamagePayload>(id).source.id == other.id) { continue; }
						if (m_ecs.hasComponent<DamagePayload>(other) && m_ecs.getComponent<DamagePayload>(other).source.id == id.id) { continue; }
						if (m_ecs.hasComponent<DamagePayload>(id)) {
							auto& payload = m_ecs.getComponent<DamagePayload>(id);
							if (m_ecs.hasComponent<Faction>(payload.source) && m_ecs.hasComponent<Faction>(other) &&
								m_ecs.getComponent<Faction>(payload.source).teamId == m_ecs.getComponent<Faction>(other).teamId) {
								continue;
							}
						}
						if (m_ecs.hasComponent<DamagePayload>(other)) {
							auto& payload = m_ecs.getComponent<DamagePayload>(other);
							if (m_ecs.hasComponent<Faction>(payload.source) && m_ecs.hasComponent<Faction>(id) &&
								m_ecs.getComponent<Faction>(payload.source).teamId == m_ecs.getComponent<Faction>(id).teamId) {
								continue;
							}
						}
						localCandidates.push_back({ id, other });
					}
				}
			});
		}

		m_threadPool.waitForAll();

		for (auto& chunk : m_chunkCandidates) {
			outCandidates.insert(outCandidates.end(), chunk.begin(), chunk.end());
		}
	}

	// Narrow phase generic sweep that uses projected position comparisons to prevent tunneling from high-velocity
	// + low-radius projectile hits
	bool CollisionSystem::narrowPhaseSwept(EntityID a, EntityID b, d64 dt, Vector2double& outHitPoint) const
	{
		auto& posA = m_ecs.getComponent<Position>(a);
		auto& posB = m_ecs.getComponent<Position>(b);
		auto& physA = m_ecs.getComponent<Physics>(a);
		auto& physB = m_ecs.getComponent<Physics>(b);

		Vector2float velA{};
		Vector2float velB{};
		if ((m_ecs.getSignature(a) & m_movementMask) == m_movementMask) { velA = m_ecs.getComponent<Movement>(a).linearVelocity; }
		if ((m_ecs.getSignature(b) & m_movementMask) == m_movementMask) { velB = m_ecs.getComponent<Movement>(b).linearVelocity; }

		Vector2double delta0 = posB.transform - posA.transform;
		Vector2double relVel{
			static_cast<d64>(velB.x - velA.x),
			static_cast<d64>(velB.y - velA.y)
		};

		d64 radiusSum = static_cast<d64>(physA.radius) + static_cast<d64>(physB.radius);
		d64 relVelSq = relVel.x * relVel.x + relVel.y * relVel.y;

		d64 t;
		if (relVelSq < 1e-12) {
			t = 0.0;
		}
		else {
			t = -(delta0.x * relVel.x + delta0.y * relVel.y) / relVelSq;
			t = std::clamp(t, 0.0, dt);
		}

		Vector2double closest{ delta0.x + relVel.x * t, delta0.y + relVel.y * t };
		d64 distSq = closest.x * closest.x + closest.y * closest.y;

		if (distSq > (radiusSum * radiusSum)) { return false; }

		Vector2double posAatT{ posA.transform.x + static_cast<d64>(velA.x) * t, posA.transform.y + static_cast<d64>(velA.y) * t };
		d64 dist = std::sqrt(distSq);
		Vector2double normal = (dist > 1e-6) ? Vector2double{ closest.x / dist, closest.y / dist } : Vector2double{ 1.0, 0.0 };
		outHitPoint = Vector2double{
			posAatT.x + normal.x * static_cast<d64>(physA.radius),
			posAatT.y + normal.y * static_cast<d64>(physA.radius)
		};

		return true;
	}


	bool CollisionSystem::narrowPhaseShip(EntityID a, EntityID b, d64 dt, Vector2double& outHitPoint) const
	{
		if (!m_ecs.hasComponent<ShipCollisionGeometry>(a) || !m_ecs.hasComponent<ShipCollisionGeometry>(b)) {
			return narrowPhaseSwept(a, b, dt, outHitPoint); // fallback for 1 or 2 non-ships
		}

		auto& geoA = m_ecs.getComponent<ShipCollisionGeometry>(a);
		auto& geoB = m_ecs.getComponent<ShipCollisionGeometry>(b);
		auto& posA = m_ecs.getComponent<Position>(a);
		auto& posB = m_ecs.getComponent<Position>(b);

		if (!convexHullsOverlap(geoA.hullSupportPoints, posA.transform, posA.rotation,
			geoB.hullSupportPoints, posB.transform, posB.rotation)) {
			return false; // SAT prefilter
		}

		EdgeHitResult hit = boundaryEdgesIntersect(geoA.boundaryEdges, posA.transform, posA.rotation,
			geoB.boundaryEdges, posB.transform, posB.rotation);
		if (!hit.hit) { return false; }

		outHitPoint = hit.point;
		return true;
	}

	void CollisionSystem::Update(d64 dt)
	{
		for (i32 idx = 0; idx < static_cast<i32>(m_proxies.size()); idx++) {
			auto& entry = m_proxies[idx];
			if (entry.proxyId == -1) { continue; }
			EntityID id = m_ecs.entityFromIndex(idx);
			if (!m_ecs.isValidEntity(id) || id.generation != entry.generation) {
				m_aabbTree.remove(entry.proxyId);
				entry.proxyId = -1;
			}
		}

		i32 c = m_ecs.sizeComponentPool<Physics>();
		for (i32 i = 0; i < c; i++) {
			i32 entityIndex = m_ecs.entityAtDenseIndex<Physics>(i);
			EntityID id = m_ecs.entityFromIndex(entityIndex);
			if ((m_ecs.getSignature(id) & m_entityMask) != m_entityMask) { continue; }

			auto& tform = m_ecs.getComponent<Position>(id);
			auto& physics = m_ecs.getComponent<Physics>(id);

			AABB bounds{
				Vector2double{ tform.transform.x - physics.radius, tform.transform.y - physics.radius },
				Vector2double{ tform.transform.x + physics.radius, tform.transform.y + physics.radius }
			};

			if (entityIndex >= static_cast<i32>(m_proxies.size())) {
				m_proxies.resize(entityIndex + 1);
			}

			auto& entry = m_proxies[entityIndex];
			if (entry.proxyId == -1 || entry.generation != id.generation) {
				entry.proxyId = m_aabbTree.insert(id, bounds);
				entry.generation = id.generation;
			}
			else {
				Vector2double displacement{};
				if (m_ecs.hasComponent<Movement>(id)) {
					auto& mv = m_ecs.getComponent<Movement>(id);
					displacement = Vector2double{ static_cast<d64>(mv.linearVelocity.x) * dt, static_cast<d64>(mv.linearVelocity.y) * dt };
				}
				m_aabbTree.moveProxy(entry.proxyId, bounds, displacement);
			}
		}

		broadPhase(m_candidates);

		m_events.clear();
		for (const auto& [a, b] : m_candidates) {
			Vector2double hitPoint{};
			bool hit = false;

			bool aIsShip = m_ecs.hasComponent<ShipCollisionGeometry>(a);
			bool bIsShip = m_ecs.hasComponent<ShipCollisionGeometry>(b);
			bool aIsProjectile = m_ecs.hasComponent<DamagePayload>(a);
			bool bIsProjectile = m_ecs.hasComponent<DamagePayload>(b);

			if (aIsShip && bIsShip) {
				hit = narrowPhaseShip(a, b, dt, hitPoint);
			}
			else if (bIsShip && aIsProjectile) {
				hit = narrowPhaseProjectileVsShip(a, b, dt, hitPoint);
			}
			else if (aIsShip && bIsProjectile) {
				hit = narrowPhaseProjectileVsShip(b, a, dt, hitPoint);
			}
			else {
				hit = narrowPhaseSwept(a, b, dt, hitPoint);
			}

			if (hit) {
				CollisionEvent event{ a, b };
				event.hitPoint = hitPoint;
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

	bool CollisionSystem::narrowPhaseProjectileVsShip(EntityID projectile, EntityID ship, d64 dt, Vector2double& outHitPoint) const
	{
		auto& shipGridData = m_ecs.getComponent<ShipGridData>(ship);
		auto& shipPos = m_ecs.getComponent<Position>(ship);
		auto& projPos = m_ecs.getComponent<Position>(projectile);

		Vector2float velocity{};
		if (m_ecs.hasComponent<Movement>(projectile)) { velocity = m_ecs.getComponent<Movement>(projectile).linearVelocity; }

		Vector2double worldStart = projPos.transform;
		Vector2double worldEnd = projPos.transform + Vector2double{ velocity.x * dt, velocity.y * dt };

		// Transform world-space start/end into ship space (inverse of the ship's Position + Rotation)
		f32 c = std::cos(-shipPos.rotation), s = std::sin(-shipPos.rotation);
		auto toLocal = [&](const Vector2double& world) -> Vector2float {
			Vector2double rel = world - shipPos.transform;
			return Vector2float{
				static_cast<f32>(rel.x * c - rel.y * s),
				static_cast<f32>(rel.x * s + rel.y * c)
			};
			};

		GridHitResult hit = raycastGrid(shipGridData, toLocal(worldStart), toLocal(worldEnd));
		if (!hit.hit) { return false; }

		// hit.point is local to ship, transform back to world space for the event.
		f32 c2 = std::cos(shipPos.rotation), s2 = std::sin(shipPos.rotation);
		outHitPoint = shipPos.transform + Vector2double{
			hit.point.x * c2 - hit.point.y * s2,
			hit.point.x * s2 + hit.point.y * c2
		};
		return true;
	}
}