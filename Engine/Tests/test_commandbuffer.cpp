#include <catch2/catch_test_macros.hpp>
#include <Utilities/CommandBuffer.h>
#include <ECS/EntityRegister.h>
#include <Core/Logger.h>
#include "TestSupport.h"

using namespace Engine;

namespace {
    struct CommandBufferFixture {
        Logger logger{ Logger::LogLevel::Debug };
        EntityRegister entityReg{ EntityRegisterDesc{ {logger}, 10 } };
        ComponentDesc compDesc{ {logger}, 10 };
        TestECSWrapper ecs{ ECSWrapperDesc{ {logger}, entityReg, compDesc } };
        CommandBuffer<TestPools> cmdBuf{ CommandBufferDesc<TestPools>{ {logger}, ecs } };
    };
}

TEST_CASE_METHOD(CommandBufferFixture, "createEntity is applied immediately, not deferred until flush", "[CommandBuffer]") {
    EntityID id = cmdBuf.createEntity();
    logger.log(Logger::LogLevel::Info, "createEntity() -> id {}, isValidEntity() before any flush(): {}", id.id, ecs.isValidEntity(id));
    REQUIRE(ecs.isValidEntity(id)); // no flush() call yet - createEntity is documented as a direct pass-through
}

TEST_CASE_METHOD(CommandBufferFixture, "addComponent is deferred until flush is called", "[CommandBuffer]") {
    EntityID id = ecs.createEntity();
    cmdBuf.addComponent(id, TestPosition{ 1.0, 2.0 });

    logger.log(Logger::LogLevel::Info, "Queued addComponent<Position>, before flush(): hasComponent={}", ecs.hasComponent<TestPosition>(id));
    REQUIRE_FALSE(ecs.hasComponent<TestPosition>(id));

    cmdBuf.flush();

    logger.log(Logger::LogLevel::Info, "After flush(): hasComponent={}, value=({}, {})",
        ecs.hasComponent<TestPosition>(id), ecs.getComponent<TestPosition>(id).x, ecs.getComponent<TestPosition>(id).y);
    REQUIRE(ecs.hasComponent<TestPosition>(id));
    REQUIRE(ecs.getComponent<TestPosition>(id).x == 1.0);
    REQUIRE(ecs.getComponent<TestPosition>(id).y == 2.0);
}

TEST_CASE_METHOD(CommandBufferFixture, "removeComponent is deferred until flush is called", "[CommandBuffer]") {
    EntityID id = ecs.createEntity();
    ecs.addComponent(id, TestPosition{});
    cmdBuf.removeComponent<TestPosition>(id);

    logger.log(Logger::LogLevel::Info, "Queued removeComponent<Position>, before flush(): hasComponent={}", ecs.hasComponent<TestPosition>(id));
    REQUIRE(ecs.hasComponent<TestPosition>(id));

    cmdBuf.flush();

    logger.log(Logger::LogLevel::Info, "After flush(): hasComponent={}", ecs.hasComponent<TestPosition>(id));
    REQUIRE_FALSE(ecs.hasComponent<TestPosition>(id));
}

TEST_CASE_METHOD(CommandBufferFixture, "destroyEntity is deferred until flush is called", "[CommandBuffer]") {
    EntityID id = ecs.createEntity();
    cmdBuf.destroyEntity(id);

    logger.log(Logger::LogLevel::Info, "Queued destroyEntity, before flush(): isValidEntity={}", ecs.isValidEntity(id));
    REQUIRE(ecs.isValidEntity(id));

    cmdBuf.flush();

    logger.log(Logger::LogLevel::Info, "After flush(): isValidEntity={}", ecs.isValidEntity(id));
    REQUIRE_FALSE(ecs.isValidEntity(id));
}

TEST_CASE_METHOD(CommandBufferFixture, "flush applies queued commands in order and clears the queue afterward", "[CommandBuffer]") {
    EntityID id = ecs.createEntity();
    cmdBuf.addComponent(id, TestPosition{ 5.0, 5.0 });
    cmdBuf.removeComponent<TestPosition>(id); // queued after the add, so the net effect after one flush should be "no component"

    cmdBuf.flush();
    logger.log(Logger::LogLevel::Info, "After flush() of [add, remove] in that order: hasComponent={}", ecs.hasComponent<TestPosition>(id));
    REQUIRE_FALSE(ecs.hasComponent<TestPosition>(id));

    // A second flush() with nothing newly queued must be a harmless no-op.
    REQUIRE_NOTHROW(cmdBuf.flush());
}
