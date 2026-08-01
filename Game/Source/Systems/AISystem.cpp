#include <Systems/AISystem.h>
#include <Components/AIController.h>
#include <Components/Position.h>
#include <Components/Movement.h>
#include <Components/Thruster.h>
#include <cmath>

namespace Engine {
	AISystem::AISystem(const AISystemDesc& desc) : Base(desc.base),
		m_ecs(desc.ecs)
	{
		m_entityMask = m_ecs.makeSignature<AIController, Position, Movement, Thruster>();
		m_reads = m_ecs.makeSignature<AIController, Position>();
		m_writes = m_ecs.makeSignature<AIController, Position, Thruster>();
		EngineLogInfo("AI system created.");
	}

	AISystem::~AISystem()
	{
		EngineLogInfo("AI system destroyed.");
	}

	EntityID AISystem::findNearestEnemy(EntityID self, i32 selfTeam, const Vector2double& selfPos) const
	{
		EntityID best{};
		d64 bestDistSq = std::numeric_limits<d64>::max();

		i32 c = m_ecs.sizeComponentPool<AIController>();
		for (i32 i = 0; i < c; i++) {
			i32 entityIndex = m_ecs.entityAtDenseIndex<AIController>(i);
			EntityID other = m_ecs.entityFromIndex(entityIndex);
			if (other.id == self.id) { continue; }
			if (!m_ecs.hasComponent<Position>(other) || !m_ecs.hasComponent<Faction>(other)) { continue; }

			if (m_ecs.getComponent<Faction>(other).teamId == selfTeam) { continue; }

			auto& otherPos = m_ecs.getComponent<Position>(other);
			Vector2double delta = otherPos.transform - selfPos;
			d64 distSq = delta.x * delta.x + delta.y * delta.y;
			if (distSq < bestDistSq) {
				bestDistSq = distSq;
				best = other;
			}
		}
		return best;
	}

	void AISystem::Update(d64 dt)
	{
		i32 c = m_ecs.sizeComponentPool<AIController>();
		for (i32 i = 0; i < c; i++) {
			i32 entityIndex = m_ecs.entityAtDenseIndex<AIController>(i);
			EntityID id = m_ecs.entityFromIndex(entityIndex);
			if ((m_ecs.getSignature(id) & m_entityMask) != m_entityMask) { continue; }

			auto& ai = m_ecs.getComponentAtDenseIndex<AIController>(i);
			auto& pos = m_ecs.getComponent<Position>(id);
			auto& movement = m_ecs.getComponent<Movement>(id);
			auto& thruster = m_ecs.getComponent<Thruster>(id);
			i32 myTeam = m_ecs.getComponent<Faction>(id).teamId;

			if (!m_ecs.isValidEntity(ai.target)) {
				ai.target = findNearestEnemy(id, myTeam, pos.transform);
			}

			if (m_ecs.isValidEntity(ai.target) && m_ecs.hasComponent<Position>(ai.target)) {
				auto& targetPos = m_ecs.getComponent<Position>(ai.target);
				Vector2double toTarget = targetPos.transform - pos.transform;
				d64 dist = std::sqrt(toTarget.x * toTarget.x + toTarget.y * toTarget.y);

				// Snap-face the target no smoothing/angularVelocity
				pos.rotation = static_cast<f32>(std::atan2(toTarget.y, toTarget.x));

				thruster.throttle = (dist > static_cast<d64>(ai.engageRange)) ? 1.0f : 0.0f;
			}
			else {
				thruster.throttle = 0.0f; // no enemies left, sit idle
			}

			i32 sc = m_ecs.sizeComponentPool<AIController>();
			for (i32 j = 0; j < sc; j++) {
				i32 otherIndex = m_ecs.entityAtDenseIndex<AIController>(j);
				EntityID other = m_ecs.entityFromIndex(otherIndex);
				if (other.id == id.id || !m_ecs.hasComponent<Position>(other) || !m_ecs.hasComponent<Faction>(other)) { continue; }

				if (m_ecs.getComponent<Faction>(other).teamId != myTeam) { continue; }

				auto& otherPos = m_ecs.getComponent<Position>(other);
				Vector2double away = pos.transform - otherPos.transform;
				d64 distSq = away.x * away.x + away.y * away.y;
				d64 sepSq = static_cast<d64>(ai.separationRadius) * static_cast<d64>(ai.separationRadius);
				if (distSq < sepSq && distSq > 1e-6) {
					d64 dist = std::sqrt(distSq);
					f32 pushStrength = 1.0f - static_cast<f32>(dist / ai.separationRadius);
					movement.linearVelocity.x += static_cast<f32>(away.x / dist) * pushStrength;
					movement.linearVelocity.y += static_cast<f32>(away.y / dist) * pushStrength;
				}
			}
		}
	}
}