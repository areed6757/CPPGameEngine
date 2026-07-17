#include <ECS/EntityRegister.h>
#include <format>

Engine::EntityRegister::EntityRegister(const EntityRegisterDesc& desc) : Base(desc.base), m_maxEntities(desc.maxEntities)
{
	m_generations.reserve(m_maxEntities +1);
	m_freeIndices.reserve(m_maxEntities +1);

	for (i32 i = m_maxEntities; i > 0; i--) {
		m_freeIndices.push_back(i);
	}

	m_generations.assign(m_maxEntities + 1, 1);
	
	EngineLogInfo(std::format("EntityRegister created, size: {}", m_freeIndices.size()).c_str());
}

Engine::EntityRegister::~EntityRegister()
{
	EngineLogInfo("EntityRegister destroyed.")
}

Engine::EntityID Engine::EntityRegister::create()
{
	if (m_freeIndices.empty()) { return EntityID{ INVALID_SENTINEL, INVALID_SENTINEL }; }
	i32 index = m_freeIndices.back();
	m_freeIndices.pop_back();

	EntityID ent = EntityID{ index, m_generations.at(index) };
	
	EngineLogDebug(std::format("Entity created: {}, generation: {}", ent.id, ent.generation).c_str());

	return ent;
}

void Engine::EntityRegister::destroy(EntityID id)
{
	if (isValid(id)) {
		EngineLogDebug(std::format("Entity destroyed id: {} generation: {}", id.id, id.generation).c_str());
		m_generations.at(id.id)++;
		m_freeIndices.push_back(id.id);
	}
	else {
		EngineLogError(std::format("Attempted to destroy invalid EntityID, id: {} generation: {}", id.id, id.generation).c_str());
	}
}

bool Engine::EntityRegister::isValid(EntityID id) const noexcept
{
	if (id.id >= m_generations.size() || id.id <= 0) { 
		return false; 
	}
	return (id.generation == m_generations[id.id]);
}

Engine::i32 Engine::EntityRegister::generationAt(i32 index) const noexcept
{
	if (index >= m_generations.size() || index <= 0) {
		return INVALID_SENTINEL;
	}
	return m_generations[index];
}