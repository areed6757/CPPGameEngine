#include <Core/Logger.h>
#include <iostream>

Engine::Logger::Logger(LogLevel logLevel) : m_logLevel(logLevel)
{
	std::clog << "|=-{ Auctoritas }-=|" << "\n";
	std::clog << "--------------------" << "\n";
}

void Engine::Logger::log(LogLevel logLevel, const char* message) const
{
	auto logLevelToString = [](LogLevel level) {
		switch (level) {
		case LogLevel::Info: return "Info";
		case LogLevel::Warning: return "Warning";
		case LogLevel::Error: return "Error";
		default: return "Unkown";
		}
		};

	if (logLevel > m_logLevel) return;
	std::clog << "[Engine " << logLevelToString(logLevel) << "]: " << message << "\n";
}

Engine::Logger::~Logger()
{
}