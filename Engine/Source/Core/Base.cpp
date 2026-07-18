#include <Core/Base.h>
#include <Core/Common.h>
using namespace Engine;

Engine::Base::Base(const BaseDesc& desc): m_logger(desc.logger)
{
}	

Engine::Base::Base(Base&& other) noexcept : m_logger(other.m_logger) {}

Engine::Base::~Base()
{
}

Logger& Engine::Base::getLogger() const noexcept
{
	return m_logger;
}