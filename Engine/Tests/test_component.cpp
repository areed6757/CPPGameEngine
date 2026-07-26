#include <catch2/catch_test_macros.hpp>
#include <ECS/Component.h>
#include <Core/Logger.h>

using namespace Engine;

namespace {
    struct TestComponent {
        i32 value{ 0 };
    };

    struct ComponentFixture {
        Logger logger{ Logger::LogLevel::Debug };
        // A small maxEntities keeps boundary-index tests meaningful without allocating
        // the default MAX_ENTITIES-sized sparse array.
        Component<TestComponent> pool{ ComponentDesc{ {logger}, 10 } };
    };
}

TEST_CASE_METHOD(ComponentFixture, "add followed by has/get exposes the stored value", "[Component]") {
    pool.add(1, TestComponent{ 42 });
    logger.log(Logger::LogLevel::Info, "Added TestComponent{{value=42}} at index 1, has(1)={}, get(1).value={}", pool.has(1), pool.get(1).value);

    REQUIRE(pool.has(1));
    REQUIRE(pool.get(1).value == 42);
    REQUIRE(pool.size() == 1);
}

TEST_CASE_METHOD(ComponentFixture, "index 0 is rejected as invalid, matching EntityRegister's 1-based indexing", "[Component]") {
    pool.add(0, TestComponent{ 7 });
    logger.log(Logger::LogLevel::Info, "add(0, ...) result: has(0)={}, size()={}", pool.has(0), pool.size());

    REQUIRE_FALSE(pool.has(0));
    REQUIRE(pool.size() == 0);
}

TEST_CASE_METHOD(ComponentFixture, "an index beyond maxEntities is rejected, and exactly maxEntities is accepted", "[Component]") {
    pool.add(11, TestComponent{ 1 }); // maxEntities is 10, so 11 is one past the valid range
    logger.log(Logger::LogLevel::Info, "add(11, ...) with maxEntities=10 result: has(11)={}", pool.has(11));
    REQUIRE_FALSE(pool.has(11));

    pool.add(10, TestComponent{ 2 }); // the boundary itself must be valid
    logger.log(Logger::LogLevel::Info, "add(10, ...) with maxEntities=10 result: has(10)={}", pool.has(10));
    REQUIRE(pool.has(10));
}

TEST_CASE_METHOD(ComponentFixture, "adding to an index that already has a component is rejected without duplicating storage", "[Component]") {
    pool.add(1, TestComponent{ 1 });
    pool.add(1, TestComponent{ 2 }); // should warn and no-op, first value must survive

    logger.log(Logger::LogLevel::Info, "After double-add at index 1: size()={}, get(1).value={}", pool.size(), pool.get(1).value);
    REQUIRE(pool.size() == 1);
    REQUIRE(pool.get(1).value == 1);
}

TEST_CASE_METHOD(ComponentFixture, "tryGet returns nullptr for indices with no component, and a valid pointer otherwise", "[Component]") {
    REQUIRE(pool.tryGet(1) == nullptr);
    REQUIRE(pool.tryGet(-1) == nullptr);   // out of range entirely
    REQUIRE(pool.tryGet(9999) == nullptr); // out of range entirely

    pool.add(1, TestComponent{ 99 });
    TestComponent* found = pool.tryGet(1);
    logger.log(Logger::LogLevel::Info, "tryGet(1) after add -> {}, value={}", static_cast<const void*>(found) != nullptr, found ? found->value : -1);

    REQUIRE(found != nullptr);
    REQUIRE(found->value == 99);
}

TEST_CASE_METHOD(ComponentFixture, "remove performs a swap-and-pop that keeps the remaining entities' data correct", "[Component]") {
    // This is the same regression concern test_ecswrapper.cpp exercises through ECSWrapper,
    // but pinned directly at the Component<T> level since Component is independently testable.
    pool.add(1, TestComponent{ 10 });
    pool.add(2, TestComponent{ 20 });
    pool.add(3, TestComponent{ 30 });

    pool.remove(2); // middle entry - forces the dense array's last element to swap into its slot

    logger.log(Logger::LogLevel::Info, "After remove(2): size()={}, get(1).value={}, get(3).value={}, has(2)={}",
        pool.size(), pool.get(1).value, pool.get(3).value, pool.has(2));

    REQUIRE(pool.size() == 2);
    REQUIRE_FALSE(pool.has(2));
    REQUIRE(pool.get(1).value == 10);
    REQUIRE(pool.get(3).value == 30);
}

TEST_CASE_METHOD(ComponentFixture, "entityAt maps a dense index back to the owning entity index", "[Component]") {
    pool.add(5, TestComponent{ 50 });
    pool.add(7, TestComponent{ 70 });

    i32 firstOwner = pool.entityAt(0);
    i32 secondOwner = pool.entityAt(1);
    logger.log(Logger::LogLevel::Info, "Dense index 0 -> entity {}, dense index 1 -> entity {}", firstOwner, secondOwner);

    REQUIRE(firstOwner == 5);
    REQUIRE(secondOwner == 7);
    REQUIRE(pool.getAtDenseIndex(0).value == 50);
    REQUIRE(pool.getAtDenseIndex(1).value == 70);
}
