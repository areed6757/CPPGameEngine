#include <catch2/catch_test_macros.hpp>
#include <Utilities/ThreadPool.h>
#include <Core/Logger.h>
#include <atomic>
#include <vector>
#include <mutex>
#include <algorithm>

using namespace Engine;

namespace {
    struct ThreadPoolLogFixture {
        Logger logger{ Logger::LogLevel::Info };
    };
}

TEST_CASE_METHOD(ThreadPoolLogFixture, "threadCount reflects the requested worker count", "[ThreadPool]") {
    ThreadPool pool{ ThreadPoolDesc{ {logger}, 4 } };
    logger.log(Logger::LogLevel::Info, "Requested 4 worker threads, pool reports threadCount() -> {}", pool.threadCount());
    REQUIRE(pool.threadCount() == 4);
}

TEST_CASE_METHOD(ThreadPoolLogFixture, "a non-positive requested thread count is clamped up to 1 worker", "[ThreadPool]") {
    ThreadPool pool{ ThreadPoolDesc{ {logger}, 0 } };
    logger.log(Logger::LogLevel::Info, "Requested 0 worker threads, pool reports threadCount() -> {}", pool.threadCount());
    REQUIRE(pool.threadCount() == 1);
}

TEST_CASE_METHOD(ThreadPoolLogFixture, "waitForAll blocks until every submitted task has completed", "[ThreadPool]") {
    ThreadPool pool{ ThreadPoolDesc{ {logger}, 4 } };
    constexpr int taskCount = 500;
    std::atomic<int> completed{ 0 };

    for (int i = 0; i < taskCount; ++i) {
        pool.submit([&completed]() { completed.fetch_add(1, std::memory_order_relaxed); });
    }
    pool.waitForAll();

    logger.log(Logger::LogLevel::Info, "Submitted {} tasks, {} completed by the time waitForAll() returned", taskCount, completed.load());
    REQUIRE(completed.load() == taskCount);
}

TEST_CASE_METHOD(ThreadPoolLogFixture, "waitForAll can be called repeatedly across separate batches of work", "[ThreadPool]") {
    ThreadPool pool{ ThreadPoolDesc{ {logger}, 4 } };
    std::atomic<int> completed{ 0 };

    for (int batch = 0; batch < 3; ++batch) {
        for (int i = 0; i < 50; ++i) {
            pool.submit([&completed]() { completed.fetch_add(1, std::memory_order_relaxed); });
        }
        pool.waitForAll();
        logger.log(Logger::LogLevel::Info, "Batch {} complete, cumulative completed count -> {}", batch, completed.load());
        REQUIRE(completed.load() == (batch + 1) * 50);
    }
}

TEST_CASE_METHOD(ThreadPoolLogFixture, "waitForAll returns immediately when no tasks have been submitted", "[ThreadPool]") {
    ThreadPool pool{ ThreadPoolDesc{ {logger}, 2 } };
    logger.log(Logger::LogLevel::Info, "Calling waitForAll() with zero submitted tasks");
    REQUIRE_NOTHROW(pool.waitForAll());
}

TEST_CASE_METHOD(ThreadPoolLogFixture, "submitted tasks each run exactly once even under contention across many workers", "[ThreadPool]") {
    ThreadPool pool{ ThreadPoolDesc{ {logger}, 8 } };
    constexpr int taskCount = 1000;
    std::mutex resultsMutex;
    std::vector<int> results;
    results.reserve(taskCount);

    for (int i = 0; i < taskCount; ++i) {
        pool.submit([&resultsMutex, &results, i]() {
            std::lock_guard<std::mutex> lock(resultsMutex);
            results.push_back(i);
            });
    }
    pool.waitForAll();

    logger.log(Logger::LogLevel::Info, "{} tasks submitted across 8 workers, {} results recorded", taskCount, results.size());
    REQUIRE(results.size() == taskCount);

    std::vector<bool> seen(taskCount, false);
    for (int v : results) { seen[v] = true; }
    bool allRanExactlyOnce = std::all_of(seen.begin(), seen.end(), [](bool b) { return b; });
    REQUIRE(allRanExactlyOnce);
}
