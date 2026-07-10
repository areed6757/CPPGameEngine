#include <ECS/EntityRegister.h>

Engine::EntityRegister::EntityRegister(const EntityRegisterDesc& desc) : Base(desc.base), m_maxEntities(desc.maxEntities)
{
}

Engine::EntityRegister::~EntityRegister()
{
}

Engine::EntityID Engine::EntityRegister::create()
{
	return EntityID();
}

void Engine::EntityRegister::destroy(EntityID& id)
{
}

bool Engine::EntityRegister::isValid(EntityID& id)
{
	return false;
}
