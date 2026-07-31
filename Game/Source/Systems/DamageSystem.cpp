#include <Systems/DamageSystem.h>

namespace Engine {
	DamageSystem::DamageSystem(const DamageSystemDesc& desc) : Base(desc.base),
		m_ecs(desc.ecs),
		m_cmdBuffer({desc.base, desc.ecs}),
		m_collisionSystem(desc.collisionSystem)
	{
		m_healthMask = m_ecs.makeSignature<Health>();
		m_damageMask = m_ecs.makeSignature<DamagePayload>();
		m_stabilityMask = m_ecs.makeSignature<Stability>();

		m_reads = m_ecs.makeSignature<Health, Stability, DamagePayload>();
		m_writes = m_ecs.makeSignature<Health, Stability>();
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
			bool aHasStability = (m_ecs.getSignature(event.entityA) & m_stabilityMask) == m_stabilityMask;
			bool bHasStability = (m_ecs.getSignature(event.entityB) & m_stabilityMask) == m_stabilityMask;

			if (aHasDamage && (bHasHealth || bHasStability)) {
				applyHit(event.entityA, event.entityB);
			}
			else if (bHasDamage && (aHasHealth || aHasStability)) {
				applyHit(event.entityB, event.entityA);
			}
		}
	}
	void DamageSystem::applyHit(EntityID damageDealer, EntityID target)
	{
		if (!m_ecs.isValidEntity(damageDealer) || !m_ecs.isValidEntity(target)) { return; }

		auto& payload = m_ecs.getComponent<DamagePayload>(damageDealer);

		if (m_ecs.hasComponent<Health>(target)) {
			auto& health = m_ecs.getComponent<Health>(target);
			health.current = std::max(0.0f, health.current - payload.amount);
			//EngineLogInfo("Entity {} dealt {} damage to entity {} ({} / {} health remaining)",
			//	damageDealer.id, payload.amount, target.id, health.current, health.max);
		}
		else if (m_ecs.hasComponent<Stability>(target)) {
			auto& stability = m_ecs.getComponent<Stability>(target);
			stability.current = std::max(0.0f, stability.current - payload.amount);
			//EngineLogInfo("Entity {} dealt {} damage to entity {} ({} / {} stability remaining)",
			//	damageDealer.id, payload.amount, target.id, stability.current, stability.max);

			if (stability.current <= 0.0f) {
				m_cmdBuffer.destroyEntity(target);
				//EngineLogInfo("Entity {} destroyed.", target.id);
			}
		}

		m_cmdBuffer.destroyEntity(damageDealer);
	}
}