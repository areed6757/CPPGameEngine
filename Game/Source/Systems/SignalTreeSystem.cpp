#include <Systems/SignalTreeSystem.h>

namespace Engine {
	SignalTreeSystem::SignalTreeSystem(const SignalTreeSystemDesc& desc) : Base(desc.base),
		m_ecs(desc.ecs),
		m_signalTree(desc.signalTree)
	{
		m_entityMask = m_ecs.makeSignature<Position, SignalSignature>();
		m_reads = m_ecs.makeSignature<Position, SignalSignature, Movement>();
		m_writes = m_ecs.makeSignature<>();
	}

	SignalTreeSystem::~SignalTreeSystem()
	{
	}

	void SignalTreeSystem::Update(d64 dt)
	{
		for (i32 idx = 0; idx < static_cast<i32>(m_proxies.size()); idx++) {
			auto& entry = m_proxies[idx];
			if (entry.proxyId == -1) { continue; }
			EntityID id = m_ecs.entityFromIndex(idx);
			if (!m_ecs.isValidEntity(id) || id.generation != entry.generation) {
				m_signalTree.remove(entry.proxyId);
				entry.proxyId = -1;
			}
		}

		i32 c = m_ecs.sizeComponentPool<SignalSignature>();
		for (i32 i = 0; i < c; i++) {
			i32 entityIndex = m_ecs.entityAtDenseIndex<SignalSignature>(i);
			EntityID id = m_ecs.entityFromIndex(entityIndex);
			if ((m_ecs.getSignature(id) & m_entityMask) != m_entityMask) { continue; }

			auto& tform = m_ecs.getComponent<Position>(id);
			AABB bounds{ tform.transform, tform.transform };

			if (entityIndex >= static_cast<i32>(m_proxies.size())) {
				m_proxies.resize(entityIndex + 1);
			}

			auto& entry = m_proxies[entityIndex];
			if (entry.proxyId == -1 || entry.generation != id.generation) {
				entry.proxyId = m_signalTree.insert(id, bounds);
				entry.generation = id.generation;
			}
			else {
				Vector2double displacement{};
				if (m_ecs.hasComponent<Movement>(id)) {
					auto& mv = m_ecs.getComponent<Movement>(id);
					displacement = Vector2double{ static_cast<d64>(mv.linearVelocity.x) * dt, static_cast<d64>(mv.linearVelocity.y) * dt };
				}
				m_signalTree.moveProxy(entry.proxyId, bounds, displacement);
			}
		}
	}
}
