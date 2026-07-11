#include <Core/Logger.h>
#include <iostream>

Engine::Logger::Logger(LogLevel logLevel) : m_logLevel(logLevel)
{
	std::clog << "|=-{ Station Authority }-=|" << "\n";
	std::clog << "---------------------------" << "\n";
}

void Engine::Logger::log(LogLevel lLevel, const char* message) const
{
	if (lLevel < m_logLevel) return;

	auto logLevelToString = [](LogLevel level) {
		switch (level) {
		case LogLevel::Debug: return "Debug";
		case LogLevel::Info: return "Info";
		case LogLevel::Warning: return "Warning";
		case LogLevel::Error: return "Error";
		default: return "Unkown";
		}
		};

	std::clog << "[Engine " << logLevelToString(lLevel) << "]: " << message << "\n";
}

Engine::Logger::~Logger()
{
}