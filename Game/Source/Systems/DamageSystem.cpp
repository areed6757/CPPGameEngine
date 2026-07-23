#include <Systems/DamageSystem.h>
#include <Components/Health.h>
#include <Components/DamagePayload.h>

namespace Engine {
	DamageSystem::DamageSystem(const DamageSystemDesc& desc) : Base(desc.base),
		m_ecs(desc.ecs),
		m_collisionSystem(desc.collisionSystem)
	{
		m_healthMask = m_ecs.makeSignature<Health>();
		m_damageMask = m_ecs.makeSignature<DamagePayload>();

		m_reads = m_ecs.makeSignature<Health, DamagePayload>();
		m_writes = m_ecs.makeSignature<Health>();
	}

	DamageSystem::~DamageSystem()
	{
	}

	void DamageSystem::Update(d64 dt)
	{
		for (const auto& event : m_collisionSystem.getEvents()) {
			if (!m_ecs.isValidEntity(event.entityA) || !m_ecs.isValidEntity(event.entityB)) { continue; }
			
			bool aHasDamage = (m_ecs.getSignature(event.entityA) & m_damageMask) == m_damageMask;
			bool bHasDamage = (m_ecs.getSignature(event.entityB) & m_damageMask) == m_damageMask;
			bool aHasHealth = (m_ecs.getSignature(event.entityA) & m_healthMask) == m_healthMask;
			bool bHasHealth = (m_ecs.getSignature(event.entityB) & m_healthMask) == m_healthMask;
		
			if (aHasDamage && bHasHealth) {
				applyHit(event.entityA, event.entityB);
			}
			else if (bHasDamage && aHasHealth) {
				applyHit(event.entityB, event.entityA);
			}
		}
	}

	void DamageSystem::applyHit(EntityID damageDealer, EntityID target)
	{
		if (!m_ecs.isValidEntity(damageDealer) || !m_ecs.isValidEntity(target)) { return; }

		auto& payload = m_ecs.getComponent<DamagePayload>(damageDealer);
		auto& health = m_ecs.getComponent<Health>(target);

		health.current -= payload.amount;

		EngineLogInfo("Entity {} dealt {} damage to entity {} ({} / {} remaining health)",
			damageDealer.id, payload.amount, target.id, health.current, health.max);

		if (health.current <= 0.0f) {
			m_ecs.destroyEntity(target);
			EngineLogInfo("Entity {} destroyed.", target.id);
		}

		m_ecs.destroyEntity(damageDealer); // Currently assumes all damaging entities are temporary projectiles
	}
}