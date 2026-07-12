#include <ECS/ECSWrapper.h>

Engine::ECSWrapper::ECSWrapper(const ECSWrapperDesc& desc) : Base(desc.base), m_entityReg(desc.entityRegister)
{
}

Engine::ECSWrapper::~ECSWrapper()
{
}
