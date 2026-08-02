#include <Systems/SeparationSystem.h>
#include <Components/Position.h>
#include <Components/Movement.h>
#include <Components/Faction.h>
#include <Components/Separation.h>
#include <cmath>

namespace Engine {
	SeparationSystem::SeparationSystem(const SeparationSystemDesc& desc) : Base(desc.base),
		m_ecs(desc.ecs), m_aabbTree(desc.aabbTree)
	{
		m_entityMask = m_ecs.makeSignature<Position, Movement, Faction, Separation>();
		m_reads = m_ecs.makeSignature<Position, Faction, Separation, Physics>();
		m_writes = m_ecs.makeSignature<Movement>();
		EngineLogInfo("Separation system created.");
	}

	SeparationSystem::~SeparationSystem()
	{
		EngineLogInfo("Separation system destroyed.");
	}

	void SeparationSystem::Update(d64 dt)
	{
		i32 c = m_ecs.sizeComponentPool<Separation>();

		m_maxRadiusSeenThisTick = 0.0f;
		for (i32 i = 0; i < c; i++) {
			i32 entityIndex = m_ecs.entityAtDenseIndex<Separation>(i);
			EntityID id = m_ecs.entityFromIndex(entityIndex);
			if (!m_ecs.hasComponent<Physics>(id)) { continue; }
			m_maxRadiusSeenThisTick = std::max(m_maxRadiusSeenThisTick, m_ecs.getComponent<Physics>(id).radius);
		}

		for (i32 i = 0; i < c; i++) {
			i32 entityIndex = m_ecs.entityAtDenseIndex<Separation>(i);
			EntityID id = m_ecs.entityFromIndex(entityIndex);
			if ((m_ecs.getSignature(id) & m_entityMask) != m_entityMask) { continue; }

			auto& sep = m_ecs.getComponentAtDenseIndex<Separation>(i);
			auto& pos = m_ecs.getComponent<Position>(id);
			auto& physics = m_ecs.getComponent<Physics>(id);
			auto& movement = m_ecs.getComponent<Movement>(id);
			i32 myTeam = m_ecs.getComponent<Faction>(id).teamId;

			f32 invMass = physics.mass > 0.0f ? 1.0f / physics.mass : 0.0f;
			d64 queryRadius = static_cast<d64>(physics.radius) + static_cast<d64>(m_maxRadiusSeenThisTick) + static_cast<d64>(sep.margin);

			AABB queryBounds{
				Vector2double{ pos.transform.x - queryRadius, pos.transform.y - queryRadius },
				Vector2double{ pos.transform.x + queryRadius, pos.transform.y + queryRadius }
			};

			m_nearbyScratch.clear();
			m_aabbTree.query(queryBounds, m_nearbyScratch);

			for (EntityID other : m_nearbyScratch) {
				if (other.id == id.id) { continue; }
				if (!m_ecs.hasComponent<Position>(other) || !m_ecs.hasComponent<Faction>(other) || !m_ecs.hasComponent<Physics>(other)) { continue; }
				if (m_ecs.getComponent<Faction>(other).teamId != myTeam) { continue; }

				auto& otherPos = m_ecs.getComponent<Position>(other);
				auto& otherPhysics = m_ecs.getComponent<Physics>(other);

				Vector2double away = pos.transform - otherPos.transform;
				d64 distSq = away.x * away.x + away.y * away.y;
				d64 combinedRadius = static_cast<d64>(physics.radius) + static_cast<d64>(otherPhysics.radius) + static_cast<d64>(sep.margin);
				d64 combinedRadiusSq = combinedRadius * combinedRadius;

				if (distSq < combinedRadiusSq && distSq > 1e-6) {
					d64 dist = std::sqrt(distSq);
					f32 pushStrength = 1.0f - static_cast<f32>(dist / combinedRadius);
					movement.linearVelocity.x += static_cast<f32>(away.x / dist) * pushStrength * invMass;
					movement.linearVelocity.y += static_cast<f32>(away.y / dist) * pushStrength * invMass;
				}
			}
		}
	}
}