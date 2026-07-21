#pragma once
#include <memory>
#include <format>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Engine {
	class Logger final {
	public:
		// Debug is VERY heavy for some functions!!
		enum class LogLevel {
			Debug = 0,
			Info,
			Warning,
			Error
		};

		explicit Logger(LogLevel logLevel = LogLevel::Error);

		template<typename... Args>
		void log(LogLevel lLevel, spdlog::format_string_t<Args...> fmt, Args&&... args) const
		{
			m_logger->log(toSpdlogLevel(lLevel), fmt, std::forward<Args>(args)...);
		}

		~Logger();

	protected:
		Logger(const Logger&) = delete;
		Logger(Logger&&) = delete;
		Logger& operator = (const Logger&) = delete;
		Logger& operator = (Logger&&) = delete;

	private:
		static spdlog::level::level_enum toSpdlogLevel(LogLevel level);
		std::unique_ptr<spdlog::logger> m_logger;
	};

	// Compile-time stripping of logger output
	#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_ERROR
	#define ENGINE_LOG_ERROR_IMPL(...) getLogger().log((Logger::LogLevel::Error), __VA_ARGS__)
	#else
	#define ENGINE_LOG_ERROR_IMPL(...) ((void)0)
	#endif
	#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_WARN
	#define ENGINE_LOG_WARNING_IMPL(...) getLogger().log((Logger::LogLevel::Warning), __VA_ARGS__)
	#else
	#define ENGINE_LOG_WARNING_IMPL(...) ((void)0)
	#endif
	#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_INFO
	#define ENGINE_LOG_INFO_IMPL(...) getLogger().log((Logger::LogLevel::Info), __VA_ARGS__)
	#else
	#define ENGINE_LOG_INFO_IMPL(...) ((void)0)
	#endif
	#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_DEBUG
	#define ENGINE_LOG_DEBUG_IMPL(...) getLogger().log((Logger::LogLevel::Debug), __VA_ARGS__)
	#else
	#define ENGINE_LOG_DEBUG_IMPL(...) ((void)0)
	#endif

	// Place enumeration vals in () to prevent unexpected behavior during macro expansion
	#define EngineLogError(...) { ENGINE_LOG_ERROR_IMPL(__VA_ARGS__); }
	#define EngineLogErrorAndThrow(...) {\
			EngineLogError(__VA_ARGS__);\
			throw std::runtime_error(std::format(__VA_ARGS__));\
			}
	#define EngineLogWarning(...) { ENGINE_LOG_WARNING_IMPL(__VA_ARGS__); }
	#define EngineLogWarningAndThrow(...) {\
			EngineLogWarning(__VA_ARGS__);\
			throw std::runtime_error(std::format(__VA_ARGS__));\
			}
	#define EngineLogInfo(...) { ENGINE_LOG_INFO_IMPL(__VA_ARGS__); }
	#define EngineLogInfoAndThrow(...) {\
			EngineLogInfo(__VA_ARGS__);\
			throw std::runtime_error(std::format(__VA_ARGS__));\
			}
	#define EngineLogDebug(...) { ENGINE_LOG_DEBUG_IMPL(__VA_ARGS__); }
}