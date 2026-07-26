#include <catch2/catch_test_macros.hpp>
#include <Utilities/JobController.h>
#include <Utilities/ThreadPool.h>
#include <Core/Logger.h>
#include <atomic>
#include <bitset>
#include <functional>
#include <thread>
#include <chrono>

using namespace Engine;

namespace {
    // Minimal TickedSystem test double: reports whatever read/write signature the test
    // wants and runs an arbitrary callback from Update(), so tests can observe execution
    // order/timing without needing a real gameplay system.
    class MockSystem : public TickedSystem {
    public:
        MockSystem(std::bitset<64> reads, std::bitset<64> writes, std::function<void()> onUpdate)
            : m_onUpdate(std::move(onUpdate)) {
            m_reads = reads;
            m_writes = writes;
        }

        void Update(d64 /*dt*/) override {
            if (m_onUpdate) { m_onUpdate(); }
            m_updateCount.fetch_add(1, std::memory_order_relaxed);
        }

        int updateCount() const noexcept { return m_updateCount.load(std::memory_order_relaxed); }

    private:
        std::function<void()> m_onUpdate;
        std::atomic<int> m_updateCount{ 0 };
    };

    struct JobControllerFixture {
        Logger logger{ Logger::LogLevel::Info };
        ThreadPool threadPool{ ThreadPoolDesc{ {logger}, 4 } };
        JobController controller{ JobControllerDesc{ {logger}, threadPool } };
    };
}

TEST_CASE_METHOD(JobControllerFixture, "runTick calls Update on every registered system exactly once", "[JobController]") {
    MockSystem a({}, std::bitset<64>{}.set(0), nullptr);
    MockSystem b({}, std::bitset<64>{}.set(1), nullptr);

    controller.runTick({ &a, &b }, 1.0 / 60.0);

    logger.log(Logger::LogLevel::Info, "After one runTick: system A updateCount={}, system B updateCount={}", a.updateCount(), b.updateCount());
    REQUIRE(a.updateCount() == 1);
    REQUIRE(b.updateCount() == 1);
}

TEST_CASE_METHOD(JobControllerFixture, "an ordering constraint guarantees the 'before' system fully finishes its batch before the 'after' system starts", "[JobController]") {
    std::atomic<int> sequence{ 0 };
    int beforeSeq = -1;
    int afterSeq = -1;

    // Disjoint write bits: nothing about their signatures alone would force ordering,
    // isolating addOrderingConstraint as the thing under test.
    MockSystem before({}, std::bitset<64>{}.set(0), [&]() { beforeSeq = sequence.fetch_add(1); });
    MockSystem after({}, std::bitset<64>{}.set(1), [&]() { afterSeq = sequence.fetch_add(1); });

    controller.addOrderingConstraint(&before, &after);
    controller.runTick({ &before, &after }, 1.0 / 60.0);

    logger.log(Logger::LogLevel::Info, "Ordering constraint result: before ran at sequence {}, after ran at sequence {}", beforeSeq, afterSeq);
    REQUIRE(beforeSeq < afterSeq);
}

TEST_CASE_METHOD(JobControllerFixture, "systems with conflicting write signatures never corrupt shared state across many ticks", "[JobController]") {
    // Both systems set bit 0 in their write signature, so JobController::conflicts() must
    // keep them in separate batches every tick. Each system does a deliberately non-atomic
    // read-sleep-write on a shared plain int; if the two ever ran concurrently despite the
    // conflict, this classic lost-update race would very likely drop increments and the
    // final total would come up short. A flaky failure here means conflict serialization regressed.
    int sharedCounter = 0;
    auto racyIncrement = [&sharedCounter]() {
        int tmp = sharedCounter;
        std::this_thread::sleep_for(std::chrono::microseconds(50));
        sharedCounter = tmp + 1;
        };

    MockSystem a(std::bitset<64>{}.set(0), std::bitset<64>{}.set(0), racyIncrement);
    MockSystem b(std::bitset<64>{}.set(0), std::bitset<64>{}.set(0), racyIncrement);

    constexpr int tickCount = 100;
    for (int i = 0; i < tickCount; ++i) {
        controller.runTick({ &a, &b }, 1.0 / 60.0);
    }

    logger.log(Logger::LogLevel::Info, "After {} ticks of two conflicting systems racing on one counter: expected {}, got {}",
        tickCount, tickCount * 2, sharedCounter);
    REQUIRE(sharedCounter == tickCount * 2);
    REQUIRE(a.updateCount() == tickCount);
    REQUIRE(b.updateCount() == tickCount);
}

TEST_CASE_METHOD(JobControllerFixture, "systems with disjoint signatures and no ordering constraint still each run exactly once per tick", "[JobController]") {
    std::atomic<int> aRuns{ 0 };
    std::atomic<int> bRuns{ 0 };
    MockSystem a({}, std::bitset<64>{}.set(0), [&]() { aRuns.fetch_add(1); });
    MockSystem b({}, std::bitset<64>{}.set(1), [&]() { bRuns.fetch_add(1); });

    constexpr int tickCount = 20;
    for (int i = 0; i < tickCount; ++i) {
        controller.runTick({ &a, &b }, 1.0 / 60.0);
    }

    logger.log(Logger::LogLevel::Info, "After {} ticks of two independent systems: A ran {} times, B ran {} times", tickCount, aRuns.load(), bRuns.load());
    REQUIRE(aRuns.load() == tickCount);
    REQUIRE(bRuns.load() == tickCount);
}
