#include <ECSWrapper.h>

Engine::ECSWrapper::ECSWrapper(const ECSWrapperDesc& desc) : Base(desc.base), m_entityReg(desc.entityRegister), m_pools{desc.compDesc}, 
m_entitySignatures(desc.compDesc.maxEntities+1) {

}

Engine::ECSWrapper::~ECSWrapper()
{
}
