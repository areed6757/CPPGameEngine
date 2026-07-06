#pragma once

namespace Engine {
	class Logger final {
	public:
		// LogLevel will eventually need debugging level to raise
		enum class LogLevel {
			Error = 0,
			Warning,
			Info
		};

		explicit Logger(LogLevel logLevel = LogLevel::Error);

		void log(LogLevel logLevel, const char* message) const;

		~Logger();

	protected:
		Logger(const Logger&) = delete;
		Logger(Logger&&) = delete;
		Logger& operator = (const Logger&) = delete;
		Logger& operator = (Logger&&) = delete;

	private:
		LogLevel m_logLevel = LogLevel::Error;
	};

	// \ at the end of macro lines indicate to preprocessor that the macro continues on the next line
	// Place enumeration vals in () to prevent unexpected behavior during macro expansion
	#define EngineLogError(message) {\
		getLogger().log((Logger::LogLevel::Error), message);\
		}

	#define EngineLogErrorAndThrow(message) {\
		EngineLogError(message);\
		throw std::runtime_error(message);\
		}

	#define EngineLogWarning(message) {\
		getLogger().log((Logger::LogLevel::Warning), message);\
		}

	#define EngineLogWarningAndThrow(message) {\
		DX3DLogWarning(message);\
		throw std::runtime_error(message);\
		}

	#define EngineLogInfo(message) {\
		getLogger().log((Logger::LogLevel::Info), message);\
		}

	#define EngineLogInfoAndThrow(message) {\
		DX3DLogInfo(message);\
		throw std::runtime_error(message);\
		}
}