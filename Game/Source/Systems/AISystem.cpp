#include <Systems/AISystem.h>
#include <Components/AIController.h>
#include <Components/Position.h>
#include <Components/Movement.h>
#include <Components/Thruster.h>
#include <cmath>

namespace Engine {
	constexpr f32 AI_CHASE_RESUME_SPEED_THRESHOLD = 0.5f;
	constexpr d64 AI_MAX_LEAD_TIME = 15.0;

	AISystem::AISystem(const AISystemDesc& desc) : Base(desc.base),
		m_ecs(desc.ecs), m_aabbTree(desc.aabbTree)
	{
		m_entityMask = m_ecs.makeSignature<AIController, Position, Movement, Thruster>();
		m_reads = m_ecs.makeSignature<AIController, Position, Movement>();
		m_writes = m_ecs.makeSignature<AIController, Position, Thruster>();
		EngineLogInfo("AI system created.");
	}

	AISystem::~AISystem()
	{
		EngineLogInfo("AI system destroyed.");
	}

	EntityID AISystem::findNearestEnemy(EntityID self, i32 selfTeam, const Vector2double& selfPos) const
	{
		constexpr d64 kInitialSearchRadius = 10.0;
		constexpr d64 kMaxSearchRadius = 1.0e6;

		for (d64 searchRadius = kInitialSearchRadius; searchRadius <= kMaxSearchRadius; searchRadius *= 2.0) {
			AABB queryBounds{
				Vector2double{ selfPos.x - searchRadius, selfPos.y - searchRadius },
				Vector2double{ selfPos.x + searchRadius, selfPos.y + searchRadius }
			};

			m_nearbyScratch.clear();
			m_aabbTree.query(queryBounds, m_nearbyScratch);

			EntityID best{};
			d64 bestDistSq = std::numeric_limits<d64>::max();

			for (EntityID other : m_nearbyScratch) {
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

			if (m_ecs.isValidEntity(best) && bestDistSq <= searchRadius * searchRadius) {
				return best;
			}
		}
		return EntityID{};
	}

	Vector2double AISystem::predictInterceptPoint(const Vector2double& selfPos, const Vector2float& selfVel,
		const Vector2double& targetPos, const Vector2float& targetVel, d64 dist) const
	{
		if (dist <= 1e-6) { return targetPos; }

		Vector2double toTarget = targetPos - selfPos;
		Vector2double relVel(targetVel);
		d64 selfSpeed = Vector2double(selfVel).length();
		if (selfSpeed <= 1e-3) { return targetPos; }

		d64 a = relVel.dot(relVel) - selfSpeed * selfSpeed;
		d64 b = 2.0 * toTarget.dot(relVel);
		d64 c = toTarget.dot(toTarget);

		d64 leadTime;
		if (std::abs(a) < 1e-6) {
			if (std::abs(b) < 1e-9) { return targetPos; }
			leadTime = -c / b;
			if (leadTime <= 0.0) { return targetPos; }
		}
		else {
			d64 disc = b * b - 4.0 * a * c;
			if (disc < 0.0) { return targetPos; }
			d64 sqrtDisc = std::sqrt(disc);
			d64 t1 = (-b + sqrtDisc) / (2.0 * a);
			d64 t2 = (-b - sqrtDisc) / (2.0 * a);

			leadTime = std::numeric_limits<d64>::max();
			if (t1 > 0.0) { leadTime = t1; }
			if (t2 > 0.0 && t2 < leadTime) { leadTime = t2; }
			if (leadTime == std::numeric_limits<d64>::max()) { return targetPos; }
		}

		leadTime = std::min(leadTime, AI_MAX_LEAD_TIME);
		return targetPos + relVel * leadTime;
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

				Vector2float targetVel = m_ecs.hasComponent<Movement>(ai.target) ?
					m_ecs.getComponent<Movement>(ai.target).linearVelocity : Vector2float{};
				Vector2double aimPoint = predictInterceptPoint(pos.transform, movement.linearVelocity,
					targetPos.transform, targetVel, dist);
				Vector2double toAimPoint = aimPoint - pos.transform;

				// Snap-face the predicted intercept point, no smoothing/angularVelocity
				pos.rotation = static_cast<f32>(std::atan2(toAimPoint.y, toAimPoint.x));

				if (dist <= static_cast<d64>(ai.engageRange)) {
					thruster.throttle = 0.0f;
					ai.throttleHeld = false;
				}
				else if (ai.throttleHeld) {

					f32 speed = std::sqrt(movement.linearVelocity.x * movement.linearVelocity.x +
						movement.linearVelocity.y * movement.linearVelocity.y);
					if (speed < AI_CHASE_RESUME_SPEED_THRESHOLD) {
						ai.throttleHeld = false;
						thruster.throttle = 1.0f;
					}
					else {
						thruster.throttle = 0.0f;
					}
				}
				else if (ai.lastTargetDist >= 0.0 && dist > ai.lastTargetDist) {
					ai.throttleHeld = true;
					thruster.throttle = 0.0f;
				}
				else {
					thruster.throttle = 1.0f;
				}

				ai.lastTargetDist = dist;
			}
			else {
				thruster.throttle = 0.0f;
				ai.lastTargetDist = -1.0;
				ai.throttleHeld = false;
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
			}
		}
	}
}