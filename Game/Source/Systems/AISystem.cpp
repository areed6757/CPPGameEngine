#include <Systems/AISystem.h>
#include <Components/AIController.h>
#include <Components/Position.h>
#include <Components/Movement.h>
#include <Components/Thruster.h>
#include <Components/Physics.h>
#include <Ships/BakedShipStats.h>
#include <cmath>

namespace Engine {
	constexpr f32 AI_CHASE_RESUME_SPEED_THRESHOLD = 0.5f;
	constexpr f32 AI_IDEAL_RANGE_TOLERANCE_FRACTION = 0.15f; // +-15% of ideal range counts as "holding position"
	// early-warning buffer, multiplied against the two ships' combined hull radius, so a turn-rate-limited
	// ship actually has room to curve away before physically touching - collisions aren't lethal yet, but
	// should mostly happen because an AI deliberately chose to ram, not by accident while just closing in
	constexpr f32 AI_COLLISION_AVOIDANCE_RADIUS_MULTIPLIER = 3.0f;

	namespace {
		f32 normalizeAngle(f32 a) {
			while (a > PI) { a -= 2.0f * PI; }
			while (a < -PI) { a += 2.0f * PI; }
			return a;
		}

		void stepTowardAngle(f32& current, f32 desired, f32 turnRate, f32 dt) {
			f32 diff = normalizeAngle(desired - current); // shortest-path delta, wraps across the +-PI seam
			f32 maxStep = turnRate * dt;
			if (std::abs(diff) <= maxStep) { current = desired; }
			else { current = normalizeAngle(current + std::copysign(maxStep, diff)); }
		}
	}

	AISystem::AISystem(const AISystemDesc& desc) : Base(desc.base),
		m_ecs(desc.ecs)
	{
		m_entityMask = m_ecs.makeSignature<AIController, Position, Movement, Thruster>();
		m_reads = m_ecs.makeSignature<AIController, Position, Movement, BakedShipStats, Physics>();
		m_writes = m_ecs.makeSignature<AIController, Position, Thruster>();
		EngineLogInfo("AI system created.");
	}

	AISystem::~AISystem()
	{
		EngineLogInfo("AI system destroyed.");
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

			if (m_ecs.isValidEntity(ai.target) && m_ecs.hasComponent<Position>(ai.target)) {
				auto& targetPos = m_ecs.getComponent<Position>(ai.target);
				Vector2double toTarget = targetPos.transform - pos.transform;
				d64 dist = std::sqrt(toTarget.x * toTarget.x + toTarget.y * toTarget.y);

				bool isFighter = m_ecs.hasComponent<BakedShipStats>(id) && m_ecs.getComponent<BakedShipStats>(id).shipClass == ShipClass::Fighter;

				f32 idealRange = ai.engageRange;
				if (m_ecs.hasComponent<BakedShipStats>(id) && m_ecs.getComponent<BakedShipStats>(id).idealRange > 0.0f) {
					idealRange = m_ecs.getComponent<BakedShipStats>(id).idealRange;
				}
				f32 tolerance = idealRange * AI_IDEAL_RANGE_TOLERANCE_FRACTION;
				d64 rangeError = dist - static_cast<d64>(idealRange); // positive: too far, negative: too close

				f32 combinedRadius = 0.0f;
				if (m_ecs.hasComponent<Physics>(id) && m_ecs.hasComponent<Physics>(ai.target)) {
					combinedRadius = m_ecs.getComponent<Physics>(id).radius + m_ecs.getComponent<Physics>(ai.target).radius;
				}
				bool collisionImminent = combinedRadius > 0.0f && dist < static_cast<d64>(combinedRadius) * static_cast<d64>(AI_COLLISION_AVOIDANCE_RADIUS_MULTIPLIER);

				bool shouldCircle = collisionImminent || (isFighter && rangeError < -static_cast<d64>(tolerance));

				f32 desiredRotation;
				if (shouldCircle) {
					// orbit tangentially instead of facing straight at the target, curving back toward it as range opens back up
					Vector2double tangent{ -toTarget.y, toTarget.x };
					desiredRotation = static_cast<f32>(std::atan2(tangent.y, tangent.x));
				}
				else {
					desiredRotation = static_cast<f32>(std::atan2(toTarget.y, toTarget.x));
				}
				stepTowardAngle(pos.rotation, desiredRotation, thruster.turnRate, static_cast<f32>(dt));

				if (shouldCircle) {
					// collision-avoidance or fighter-circling take priority over holding position, even if
					// idealRange's tolerance band would otherwise say "stop here" - never call a collision
					// course "in position"
					thruster.throttle = 1.0f;
					ai.throttleHeld = false;
				}
				else if (std::abs(rangeError) <= static_cast<d64>(tolerance)) {
					thruster.throttle = 0.0f;
					ai.throttleHeld = false;
				}
				else if (rangeError < 0.0) {
					// too close (non-fighter), back off while still facing (and firing at) the target
					thruster.throttle = -1.0f;
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