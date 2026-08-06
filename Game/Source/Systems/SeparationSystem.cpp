#include <Systems/SeparationSystem.h>
#include <Components/Position.h>
#include <Components/Movement.h>
#include <Components/Faction.h>
#include <Components/Separation.h>
#include <Components/Thruster.h>
#include <cmath>

namespace Engine {
	// Thruster::maxVelocity <= 0 means uncapped, this stands in as a bound large enough that solveORCA's clamp is effectively a no-op
	constexpr f32 SEPARATION_UNCAPPED_MAX_SPEED = 1.0e4f;

	SeparationSystem::SeparationSystem(const SeparationSystemDesc& desc) : Base(desc.base),
		m_ecs(desc.ecs), m_aabbTree(desc.aabbTree)
	{
		m_entityMask = m_ecs.makeSignature<Position, Movement, Faction, Separation>();
		m_reads = m_ecs.makeSignature<Position, Movement, Faction, Separation, Physics, Thruster>();
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
		f32 fdt = static_cast<f32>(dt);

		m_maxRadiusSeenThisTick = 0.0f;
		m_maxSpeedSeenThisTick = 0.0f;
		for (i32 i = 0; i < c; i++) {
			i32 entityIndex = m_ecs.entityAtDenseIndex<Separation>(i);
			EntityID id = m_ecs.entityFromIndex(entityIndex);
			if (!m_ecs.hasComponent<Physics>(id)) { continue; }
			m_maxRadiusSeenThisTick = std::max(m_maxRadiusSeenThisTick, m_ecs.getComponent<Physics>(id).radius);
			if (m_ecs.hasComponent<Movement>(id)) {
				m_maxSpeedSeenThisTick = std::max(m_maxSpeedSeenThisTick, m_ecs.getComponent<Movement>(id).linearVelocity.length());
			}
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

			// query box sized off tick-wide max radius/speed so fast-closing ships are found early, per-pair check below discards the irrelevant ones
			d64 queryRadius = static_cast<d64>(physics.radius) + static_cast<d64>(m_maxRadiusSeenThisTick) + static_cast<d64>(sep.margin) +
				static_cast<d64>(sep.timeHorizon) * static_cast<d64>(movement.linearVelocity.length() + m_maxSpeedSeenThisTick);

			AABB queryBounds{
				Vector2double{ pos.transform.x - queryRadius, pos.transform.y - queryRadius },
				Vector2double{ pos.transform.x + queryRadius, pos.transform.y + queryRadius }
			};

			m_nearbyScratch.clear();
			m_aabbTree.query(queryBounds, m_nearbyScratch);

			m_orcaLines.clear();
			for (EntityID other : m_nearbyScratch) {
				if (other.id == id.id) { continue; }
				if (!m_ecs.hasComponent<Position>(other) || !m_ecs.hasComponent<Faction>(other) ||
					!m_ecs.hasComponent<Physics>(other) || !m_ecs.hasComponent<Movement>(other)) { continue; }
				if (m_ecs.getComponent<Faction>(other).teamId != myTeam) { continue; }

				auto& otherPos = m_ecs.getComponent<Position>(other);
				auto& otherPhysics = m_ecs.getComponent<Physics>(other);
				auto& otherMovement = m_ecs.getComponent<Movement>(other);

				Vector2double relD = otherPos.transform - pos.transform;
				f32 combinedRadius = physics.radius + otherPhysics.radius + sep.margin;

				// this pair's own relevant range, skips neighbors the shared query box over-fetched but aren't a real threat
				f32 relevantRange = combinedRadius + sep.timeHorizon * (movement.linearVelocity.length() + otherMovement.linearVelocity.length());
				if (relD.x * relD.x + relD.y * relD.y > static_cast<d64>(relevantRange) * static_cast<d64>(relevantRange)) { continue; }

				Vector2float relativePosition{ static_cast<f32>(relD.x), static_cast<f32>(relD.y) };

				// mass <= 0 is immovable (Physics.h), self stays put regardless of the neighbor, or dodges alone if the neighbor is the immovable one, otherwise splits by inverse mass ratio
				f32 selfResponsibility;
				if (physics.mass <= 0.0f) { selfResponsibility = 0.0f; }
				else if (otherPhysics.mass <= 0.0f) { selfResponsibility = 1.0f; }
				else { selfResponsibility = otherPhysics.mass / (physics.mass + otherPhysics.mass); }

				m_orcaLines.push_back(computeORCALine(movement.linearVelocity, relativePosition, otherMovement.linearVelocity, combinedRadius, sep.timeHorizon, fdt, selfResponsibility));
			}

			f32 maxSpeed = SEPARATION_UNCAPPED_MAX_SPEED;
			if (m_ecs.hasComponent<Thruster>(id) && m_ecs.getComponent<Thruster>(id).maxVelocity > 0.0f) {
				maxSpeed = m_ecs.getComponent<Thruster>(id).maxVelocity;
			}

			movement.linearVelocity = solveORCA(m_orcaLines, movement.linearVelocity, maxSpeed);
		}
	}
}