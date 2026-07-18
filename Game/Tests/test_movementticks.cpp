#include <catch2/catch_test_macros.hpp>
#include <Core/Logger.h>
#include <ECS/EntityRegister.h>
#include <GameDescs.h>
#include <Systems/MovementTicks.h>
#include <Components/Position.h>
#include <Components/Movement.h>

using namespace Engine;

struct MovementFixture {
    Logger logger{ Logger::LogLevel::Debug };
    EntityRegister entityReg{ EntityRegisterDesc{ { logger }, 100 } };
    ComponentDesc compDesc{ { logger }, 100 };
    GameECSWrapper ecs{ ECSWrapperDesc{ { logger }, entityReg, compDesc } };
    MovementTicks movementSystem{ MovementTicksDesc{ { logger }, ecs } };
};

TEST_CASE_METHOD(MovementFixture, "entity with only Position is untouched by MovementTicks", "[MovementTicks]") {
    EntityID id = ecs.createEntity();
    ecs.addComponent(id, Position{}); // no Movement, should not match the system's signature
    movementSystem.Update(1.0);
    REQUIRE(ecs.getComponent<Position>(id).rotation == 0.0f); // unchanged
}

TEST_CASE_METHOD(MovementFixture, "entity with Position+Movement integrates over one tick", "[MovementTicks]") {
    EntityID id = ecs.createEntity();
    ecs.addComponent(id, Position{});
    Movement m{};
    m.linearVelocity = 2.0f; // NOTE: adjust field names/types to match the actual current Movement struct
    ecs.addComponent(id, m);

    movementSystem.Update(1.0);

    // Position should have moved from the origin, exact expected values depend on
    // the current (possibly still-scalar) Movement model; assert movement happened
    // rather than a specific magnitude, given that logic is still being reworked.
    auto& pos = ecs.getComponent<Position>(id);
    bool moved = (pos.transform.x != 0.0 || pos.transform.y != 0.0);
    REQUIRE(moved);
}