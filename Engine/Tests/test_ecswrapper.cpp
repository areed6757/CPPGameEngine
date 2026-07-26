#include <catch2/catch_test_macros.hpp>
#include <ECS/EntityRegister.h>
#include <Core/Logger.h>
#include <bitset>
#include "TestSupport.h"

using namespace Engine;

namespace {
    struct EngineECSFixture {
        Logger logger{ Logger::LogLevel::Debug };
        EntityRegister entityReg{ EntityRegisterDesc{ {logger}, 10 } };
        ComponentDesc compDesc{ {logger}, 10 };
        TestECSWrapper ecs{ ECSWrapperDesc{ {logger}, entityReg, compDesc } };
    };
}

TEST_CASE_METHOD(EngineECSFixture, "makeSignature with multiple component types ORs their bits together", "[ECSWrapper]") {
    auto sigPos = ecs.makeSignature<TestPosition>();
    auto sigBoth = ecs.makeSignature<TestPosition, TestVelocity>();
    logger.log(Logger::LogLevel::Info, "makeSignature<Position>() -> {}, makeSignature<Position,Velocity>() -> {}",
        sigPos.to_string(), sigBoth.to_string());

    REQUIRE(sigBoth.count() == 2);
    REQUIRE(sigBoth.test(ComponentBit<TestPosition>::value));
    REQUIRE(sigBoth.test(ComponentBit<TestVelocity>::value));
    REQUIRE((sigBoth & sigPos) == sigPos); // combined signature is a superset of the single-component one
}

TEST_CASE_METHOD(EngineECSFixture, "getSignature reflects exactly the components currently attached to an entity", "[ECSWrapper]") {
    EntityID id = ecs.createEntity();
    ecs.addComponent(id, TestPosition{});
    ecs.addComponent(id, TestVelocity{});

    auto sig = ecs.getSignature(id);
    logger.log(Logger::LogLevel::Info, "Entity {} signature after adding Position+Velocity: {}", id.id, sig.to_string());
    REQUIRE(sig == ecs.makeSignature<TestPosition, TestVelocity>());

    ecs.removeComponent<TestVelocity>(id);
    sig = ecs.getSignature(id);
    logger.log(Logger::LogLevel::Info, "Entity {} signature after removing Velocity: {}", id.id, sig.to_string());
    REQUIRE(sig == ecs.makeSignature<TestPosition>());
}

TEST_CASE_METHOD(EngineECSFixture, "destroying and recreating an entity reuses its index but bumps the generation, invalidating the old id", "[ECSWrapper]") {
    EntityID first = ecs.createEntity();
    ecs.addComponent(first, TestPosition{ 1.0, 1.0 });
    ecs.destroyEntity(first);

    EntityID second = ecs.createEntity(); // free-list is LIFO, so this reuses `first`'s index immediately

    logger.log(Logger::LogLevel::Info, "first = (id={}, gen={}), second = (id={}, gen={})",
        first.id, first.generation, second.id, second.generation);

    REQUIRE(second.id == first.id);
    REQUIRE(second.generation == first.generation + 1);
    REQUIRE_FALSE(ecs.isValidEntity(first));
    REQUIRE(ecs.isValidEntity(second));
    REQUIRE_FALSE(ecs.hasComponent<TestPosition>(second)); // destroyEntity must have cleared the old component
}

TEST_CASE_METHOD(EngineECSFixture, "destroyEntity on an already-invalid id is a safe no-op", "[ECSWrapper]") {
    EntityID id = ecs.createEntity();
    ecs.destroyEntity(id);

    logger.log(Logger::LogLevel::Info, "Calling destroyEntity a second time on already-destroyed id {} (should be a harmless no-op)", id.id);
    REQUIRE_NOTHROW(ecs.destroyEntity(id));
}

TEST_CASE_METHOD(EngineECSFixture, "sizeComponentPool and entityAtDenseIndex track each component type's pool independently", "[ECSWrapper]") {
    EntityID a = ecs.createEntity();
    EntityID b = ecs.createEntity();

    ecs.addComponent(a, TestPosition{});
    ecs.addComponent(a, TestVelocity{});
    ecs.addComponent(b, TestPosition{});

    logger.log(Logger::LogLevel::Info, "Position pool size={}, Velocity pool size={}",
        ecs.sizeComponentPool<TestPosition>(), ecs.sizeComponentPool<TestVelocity>());

    REQUIRE(ecs.sizeComponentPool<TestPosition>() == 2);
    REQUIRE(ecs.sizeComponentPool<TestVelocity>() == 1);
    REQUIRE(ecs.entityAtDenseIndex<TestVelocity>(0) == a.id);
}

TEST_CASE_METHOD(EngineECSFixture, "tryGetComponent returns nullptr for an entity without the component and a valid pointer otherwise", "[ECSWrapper]") {
    EntityID id = ecs.createEntity();
    REQUIRE(ecs.tryGetComponent<TestPosition>(id) == nullptr);

    ecs.addComponent(id, TestPosition{ 3.0, 4.0 });
    TestPosition* pos = ecs.tryGetComponent<TestPosition>(id);
    logger.log(Logger::LogLevel::Info, "tryGetComponent<Position> after add -> non-null={}, ({}, {})",
        pos != nullptr, pos ? pos->x : 0.0, pos ? pos->y : 0.0);

    REQUIRE(pos != nullptr);
    REQUIRE(pos->x == 3.0);
    REQUIRE(pos->y == 4.0);
}
