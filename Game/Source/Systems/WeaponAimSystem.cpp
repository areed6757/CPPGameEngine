#include <Systems/WeaponAimSystem.h>
#include <cmath>

namespace Engine {
	constexpr d64 WEAPON_AIM_MAX_LEAD_TIME = 15.0;

	namespace {
		f32 normalizeAngle(f32 a) {
			while (a > PI) { a -= 2.0f * PI; }
			while (a < -PI) { a += 2.0f * PI; }
			return a;
		}

		void stepTowardAngle(f32& current, f32 desired, f32 traverseSpeed, f32 dt) {
			f32 diff = normalizeAngle(desired - current); // shortest-path delta, wraps across the +-PI seam
			f32 maxStep = traverseSpeed * dt;
			if (std::abs(diff) <= maxStep) { current = desired; }
			else { current = normalizeAngle(current + std::copysign(maxStep, diff)); }
		}
	}

	WeaponAimSystem::WeaponAimSystem(const WeaponAimSystemDesc& desc) : Base(desc.base),
		m_ecs(desc.ecs), m_aabbTree(desc.aabbTree)
	{
		m_entityMask = m_ecs.makeSignature<Mount, Weapon>();
		m_reads = m_ecs.makeSignature<Mount, Weapon, Position, Movement, Faction>();
		m_writes = m_ecs.makeSignature<Weapon>();
	}

	WeaponAimSystem::~WeaponAimSystem()
	{
	}

	EntityID WeaponAimSystem::findNearestEnemy(EntityID self, i32 selfTeam, const Vector2double& selfPos) const
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

	Vector2double WeaponAimSystem::predictInterceptPoint(const Vector2double& selfPos, f32 projectileSpeed,
		const Vector2double& targetPos, const Vector2float& targetVel, d64 dist) const
	{
		if (dist <= 1e-6) { return targetPos; }

		Vector2double toTarget = targetPos - selfPos;
		Vector2double relVel(targetVel);
		d64 selfSpeed = static_cast<d64>(projectileSpeed);
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

		leadTime = std::min(leadTime, WEAPON_AIM_MAX_LEAD_TIME);
		return targetPos + relVel * leadTime;
	}

	void WeaponAimSystem::Update(d64 dt)
	{
		i32 c = m_ecs.sizeComponentPool<Weapon>();
		for (i32 i = 0; i < c; i++) {
			i32 entityIndex = m_ecs.entityAtDenseIndex<Weapon>(i);
			EntityID id = m_ecs.entityFromIndex(entityIndex);
			if ((m_ecs.getSignature(id) & m_entityMask) != m_entityMask) { continue; }

			auto& mount = m_ecs.getComponent<Mount>(id);
			if (!m_ecs.isValidEntity(mount.owner) || !m_ecs.hasComponent<Position>(mount.owner)) { continue; }

			auto& weapon = m_ecs.getComponentAtDenseIndex<Weapon>(i);
			auto& ownerPos = m_ecs.getComponent<Position>(mount.owner);

			if (!m_ecs.hasComponent<Faction>(mount.owner)) {
				weapon.targetInRange = false;
				stepTowardAngle(weapon.aimRotation, weapon.restRotationAbs, weapon.traverseSpeed, static_cast<f32>(dt));
				continue;
			}

			i32 myTeam = m_ecs.getComponent<Faction>(mount.owner).teamId;

			f32 shipRot = ownerPos.rotation;
			Vector2float facing{ std::cos(shipRot), std::sin(shipRot) };
			Vector2double anchorWorldPos = ownerPos.transform + Vector2double{
				mount.offset.x * facing.x - mount.offset.y * facing.y,
				mount.offset.x * facing.y + mount.offset.y * facing.x
			};

			EntityID target = findNearestEnemy(mount.owner, myTeam, anchorWorldPos);
			if (!m_ecs.isValidEntity(target) || !m_ecs.hasComponent<Position>(target)) {
				weapon.targetInRange = false;
				stepTowardAngle(weapon.aimRotation, weapon.restRotationAbs, weapon.traverseSpeed, static_cast<f32>(dt));
				continue;
			}

			auto& targetPos = m_ecs.getComponent<Position>(target);
			Vector2double toTarget = targetPos.transform - anchorWorldPos;
			d64 dist = std::sqrt(toTarget.x * toTarget.x + toTarget.y * toTarget.y);

			weapon.targetInRange = dist <= static_cast<d64>(weapon.maxRange);

			Vector2float targetVel = m_ecs.hasComponent<Movement>(target) ?
				m_ecs.getComponent<Movement>(target).linearVelocity : Vector2float{};

			Vector2double aimPoint = predictInterceptPoint(anchorWorldPos, weapon.projectileSpeed, targetPos.transform, targetVel, dist);
			Vector2double toAimPoint = aimPoint - anchorWorldPos;

			f32 desiredWorldAngle = static_cast<f32>(std::atan2(toAimPoint.y, toAimPoint.x));
			f32 desiredShipRelative = normalizeAngle(desiredWorldAngle - shipRot);
			f32 desiredClamped = std::clamp(desiredShipRelative, weapon.minRotationAbs, weapon.maxRotationAbs);

			stepTowardAngle(weapon.aimRotation, desiredClamped, weapon.traverseSpeed, static_cast<f32>(dt));
		}
	}
}
