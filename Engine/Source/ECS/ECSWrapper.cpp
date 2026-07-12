#include <ECS/ECSWrapper.h>

Engine::ECSWrapper::ECSWrapper(const ECSWrapperDesc& desc) : Base(desc.base), m_entityReg(desc.entityRegister), m_pools{desc.compDesc} {
}

Engine::ECSWrapper::~ECSWrapper()
{
}
