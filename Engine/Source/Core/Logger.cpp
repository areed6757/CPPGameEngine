#include <Core/Logger.h>
#include <spdlog/sinks/rotating_file_sink.h>

namespace Engine {
	
	spdlog::level::level_enum Logger::toSpdlogLevel(LogLevel level)
	{
		switch (level) {
		case LogLevel::Debug:   return spdlog::level::debug;
		case LogLevel::Info:    return spdlog::level::info;
		case LogLevel::Warning: return spdlog::level::warn;
		case LogLevel::Error:   return spdlog::level::err;
		default:                return spdlog::level::info;
		}
	}

	Logger::Logger(LogLevel logLevel)
	{
		auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
			"logs/engine.log", 1024 * 1024 * 5, 3); // 5MB per file, 3 rotated

		m_logger = std::make_unique<spdlog::logger>(
			"engine", spdlog::sinks_init_list{ console_sink, file_sink });

		m_logger->set_pattern("[Engine %^%l%$]: %v");
		m_logger->set_level(toSpdlogLevel(logLevel));

		m_logger->info("||=-{{ Stacked Engine }}-=||");
		m_logger->info("----------------------------");
	}

	Logger::~Logger()
	{
		m_logger->info("Logger destroyed.");
	}



}