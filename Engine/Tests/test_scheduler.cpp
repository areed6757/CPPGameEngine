#include <catch2/catch_test_macros.hpp>
#include <Utilities/Scheduler.h>
#include <Utilities/GameClock.h>
#include <Utilities/JobController.h>
#include <Utilities/ThreadPool.h>
#include <ECS/TickedSystem.h>
#include <Core/Common.h>
#include <Core/Logger.h>
#include <atomic>

using namespace Engine;

namespace {
    class CountingSystem : public TickedSystem {
    public:
        void Update(d64 /*dt*/) override { m_count.fetch_add(1, std::memory_order_relaxed); }
        int count() const noexcept { return m_count.load(std::memory_order_relaxed); }
    private:
        std::atomic<int> m_count{ 0 };
    };

    struct SchedulerFixture {
        Logger logger{ Logger::LogLevel::Info };
        GameClock clock{ GameClockDesc{ {logger} } };
        ThreadPool threadPool{ ThreadPoolDesc{ {logger}, 2 } };
        JobController jobController{ JobControllerDesc{ {logger}, threadPool } };
    };
}

TEST_CASE_METHOD(SchedulerFixture, "a registered ticked system runs once real time has accumulated past the fixed timestep", "[Scheduler]") {
    // A microsecond-scale fixed timestep means essentially any measurable dt on the very
    // first advance() call is enough to trigger a tick, keeping this deterministic instead
    // of relying on sleeping for a real ~16ms frame.
    Scheduler scheduler{ SchedulerDesc{ {logger}, clock, jobController, 1e-6 } };
    CountingSystem tickedSys;
    scheduler.registerSystem(&tickedSys);

    scheduler.advance();

    logger.log(Logger::LogLevel::Info, "Ticked system updateCount after one advance() with a 1us fixed timestep: {}", tickedSys.count());
    REQUIRE(tickedSys.count() >= 1);
}

TEST_CASE_METHOD(SchedulerFixture, "a registered frame system runs on every advance() call regardless of the fixed-timestep accumulator", "[Scheduler]") {
    Scheduler scheduler{ SchedulerDesc{ {logger}, clock, jobController, TICK_RATE } }; // normal ~60Hz timestep, deliberately not tiny
    CountingSystem frameSys;
    scheduler.registerFrameSystem(&frameSys);

    scheduler.advance();
    scheduler.advance();
    scheduler.advance();

    logger.log(Logger::LogLevel::Info, "Frame system updateCount after 3 advance() calls: {}", frameSys.count());
    REQUIRE(frameSys.count() == 3);
}

TEST_CASE_METHOD(SchedulerFixture, "togglePause halts fixed-timestep ticked systems but frame systems keep running every advance()", "[Scheduler]") {
    Scheduler scheduler{ SchedulerDesc{ {logger}, clock, jobController, TICK_RATE } };
    CountingSystem tickedSys;
    CountingSystem frameSys;
    scheduler.registerSystem(&tickedSys);
    scheduler.registerFrameSystem(&frameSys);

    scheduler.togglePause(); // accumulator is reset to 0 on every advance() while paused, so the tick loop can never fire

    scheduler.advance();
    scheduler.advance();

    logger.log(Logger::LogLevel::Info, "While paused, over 2 advance() calls: ticked system ran {} times, frame system ran {} times",
        tickedSys.count(), frameSys.count());
    REQUIRE(tickedSys.count() == 0);
    REQUIRE(frameSys.count() == 2);

    scheduler.togglePause(); // leave it unpaused for hygiene, even though the object is about to go out of scope
}

TEST_CASE_METHOD(SchedulerFixture, "registerFlushCallback fires exactly once for every fixed-timestep tick that runs", "[Scheduler]") {
    Scheduler scheduler{ SchedulerDesc{ {logger}, clock, jobController, 1e-6 } };
    CountingSystem tickedSys;
    scheduler.registerSystem(&tickedSys);

    int flushCount = 0;
    scheduler.registerFlushCallback([&flushCount]() { flushCount++; });

    scheduler.advance();
    scheduler.advance();

    logger.log(Logger::LogLevel::Info, "After 2 advance() calls: ticked system ran {} times, flush callback ran {} times", tickedSys.count(), flushCount);
    REQUIRE(flushCount == tickedSys.count());
    REQUIRE(flushCount > 0);
}
