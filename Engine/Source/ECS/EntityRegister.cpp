#include <ECS/EntityRegister.h>
#include <format>

namespace Engine {
	EntityRegister::EntityRegister(const EntityRegisterDesc& desc) : Base(desc.base), m_maxEntities(desc.maxEntities)
	{
		m_generations.reserve(m_maxEntities + 1);
		m_freeIndices.reserve(m_maxEntities + 1);

		for (i32 i = m_maxEntities; i > 0; i--) {
			m_freeIndices.push_back(i);
		}

		m_generations.assign(m_maxEntities + 1, 1);

		EngineLogInfo("EntityRegister created, size: {}", m_freeIndices.size());
	}

	EntityRegister::~EntityRegister()
	{
		EngineLogInfo("EntityRegister destroyed.")
	}

	EntityID EntityRegister::create()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		if (m_freeIndices.empty()) { return EntityID{ INVALID_SENTINEL, INVALID_SENTINEL }; }
		i32 index = m_freeIndices.back();
		m_freeIndices.pop_back();

		EntityID ent = EntityID{ index, m_generations.at(index) };

		EngineLogDebug("Entity created: {}, generation: {}", ent.id, ent.generation);

		return ent;
	}

	void EntityRegister::destroy(EntityID id)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		if (isValid(id)) {
			EngineLogDebug("Entity destroyed id: {} generation: {}", id.id, id.generation);
			m_generations[id.id]++;
			m_freeIndices.push_back(id.id);
		}
		else {
			EngineLogError("Attempted to destroy invalid EntityID, id: {} generation: {}", id.id, id.generation);
		}
	}

	bool EntityRegister::isValid(EntityID id) const noexcept
	{
		if (id.id >= m_generations.size() || id.id <= 0) {
			return false;
		}
		return (id.generation == m_generations[id.id]);
	}

	i32 EntityRegister::generationAt(i32 index) const noexcept
	{
		if (index >= m_generations.size() || index <= 0) {
			return INVALID_SENTINEL;
		}
		return m_generations[index];
	}
}