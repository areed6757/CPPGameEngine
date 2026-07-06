#include <Core/Base.h>
using namespace Engine;

Engine::Base::Base(const BaseDesc& desc): m_logger(desc.logger)
{
}	

Engine::Base::~Base()
{
}

Logger& Engine::Base::getLogger() const noexcept
{
	return m_logger;
}