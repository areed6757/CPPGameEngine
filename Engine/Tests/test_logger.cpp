#include <catch2/catch_test_macros.hpp>
#include <Core/Logger.h>

using namespace Engine;

// Logger writes to stdout and a rotating file sink ("logs/engine.log" relative to the
// test executable's working directory) as a side effect of construction/logging - that's
// expected here, matching how the existing Game/Tests fixtures already use Logger.

TEST_CASE("Logger constructs and destructs cleanly at every LogLevel", "[Logger]") {
    REQUIRE_NOTHROW(Logger{ Logger::LogLevel::Debug });
    REQUIRE_NOTHROW(Logger{ Logger::LogLevel::Info });
    REQUIRE_NOTHROW(Logger{ Logger::LogLevel::Warning });
    REQUIRE_NOTHROW(Logger{ Logger::LogLevel::Error });
    REQUIRE_NOTHROW(Logger{}); // default level is LogLevel::Error
}

TEST_CASE("Logger::log accepts fmt-style {} placeholders at every level without throwing", "[Logger]") {
    Logger logger{ Logger::LogLevel::Debug };

    REQUIRE_NOTHROW(logger.log(Logger::LogLevel::Debug, "debug message, no args"));
    REQUIRE_NOTHROW(logger.log(Logger::LogLevel::Info, "entity {} created with {} components", 42, 3));
    REQUIRE_NOTHROW(logger.log(Logger::LogLevel::Warning, "pool at {}/{} capacity", 950, 1000));
    REQUIRE_NOTHROW(logger.log(Logger::LogLevel::Error, "failed to load '{}': {}", "ship.mesh", "file not found"));

    // Format-spec placeholders (as used by JobController's tick-time average log) should also work.
    REQUIRE_NOTHROW(logger.log(Logger::LogLevel::Info, "avg tick time over {} ticks: {:.3f}ms", 60, 4.2837));
}

TEST_CASE("Logger constructed at a low severity does not filter out higher-severity log calls", "[Logger]") {
    // LogLevel here only tags the source of the call; spdlog's active level is controlled
    // at compile time via SPDLOG_ACTIVE_LEVEL (see Engine/CMakeLists.txt), not by this ctor
    // argument, so this should never throw regardless of the level requested at construction.
    Logger logger{ Logger::LogLevel::Error };
    REQUIRE_NOTHROW(logger.log(Logger::LogLevel::Debug, "debug call on an Error-level logger"));
    REQUIRE_NOTHROW(logger.log(Logger::LogLevel::Error, "matching-level call"));
}

TEST_CASE("Multiple independent Logger instances can coexist", "[Logger]") {
    Logger a{ Logger::LogLevel::Info };
    Logger b{ Logger::LogLevel::Debug };

    REQUIRE_NOTHROW(a.log(Logger::LogLevel::Info, "message from logger a"));
    REQUIRE_NOTHROW(b.log(Logger::LogLevel::Debug, "message from logger b"));
}
