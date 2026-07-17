#include <catch2/catch_test_macros.hpp>
#include <Core/Logger.h>
#include <ECS/EntityRegister.h>
#include <ECSWrapper.h>
#include <Components/Position.h>
#include <Components/Movement.h>

using namespace Engine;

struct WrapperFixture {
    Logger logger{ Logger::LogLevel::Debug };
    EntityRegister entityReg{ EntityRegisterDesc{ { logger }, 100 } };
    ComponentDesc compDesc{ { logger }, 100 };
    ECSWrapper ecs{ ECSWrapperDesc{ { logger }, entityReg, compDesc } };
};

TEST_CASE_METHOD(WrapperFixture, "addComponent sets the signature bit and hasComponent reports true", "[ECSWrapper]") {
    EntityID id = ecs.createEntity();
    REQUIRE_FALSE(ecs.hasComponent<Position>(id));

    ecs.addComponent(id, Position{});
    REQUIRE(ecs.hasComponent<Position>(id));
    REQUIRE((ecs.getSignature(id) & ecs.makeSignature<Position>()) == ecs.makeSignature<Position>());
}

TEST_CASE_METHOD(WrapperFixture, "removeComponent clears the signature bit", "[ECSWrapper]") {
    EntityID id = ecs.createEntity();
    ecs.addComponent(id, Position{});
    ecs.removeComponent<Position>(id);
    REQUIRE_FALSE(ecs.hasComponent<Position>(id));
}

TEST_CASE_METHOD(WrapperFixture, "getComponent returns a mutable reference that persists", "[ECSWrapper]") {
    EntityID id = ecs.createEntity();
    ecs.addComponent(id, Position{});
    ecs.getComponent<Position>(id).rotation = 1.5f;
    REQUIRE(ecs.getComponent<Position>(id).rotation == 1.5f);
}

TEST_CASE_METHOD(WrapperFixture, "swap-and-pop keeps remaining components correct after removal", "[ECSWrapper]") {
    // Add Position to three entities, remove the middle one, confirm the other two
    // still resolve to their own correct data, this is the real regression test
    // for Component<T>'s swap-and-pop implementation, exercised through the public API.
    EntityID a = ecs.createEntity();
    EntityID b = ecs.createEntity();
    EntityID c = ecs.createEntity();

    Position posA{}; posA.rotation = 1.0f;
    Position posB{}; posB.rotation = 2.0f;
    Position posC{}; posC.rotation = 3.0f;
    ecs.addComponent(a, posA);
    ecs.addComponent(b, posB);
    ecs.addComponent(c, posC);

    ecs.removeComponent<Position>(b);

    REQUIRE_FALSE(ecs.hasComponent<Position>(b));
    REQUIRE(ecs.getComponent<Position>(a).rotation == 1.0f);
    REQUIRE(ecs.getComponent<Position>(c).rotation == 3.0f);
}

TEST_CASE_METHOD(WrapperFixture, "destroyEntity removes all components and invalidates the id", "[ECSWrapper]") {
    EntityID id = ecs.createEntity();
    ecs.addComponent(id, Position{});
    ecs.addComponent(id, Movement{});

    ecs.destroyEntity(id);

    REQUIRE_FALSE(ecs.isValidEntity(id));
    REQUIRE(ecs.getSignature(id).none()); // no bits left set
}

TEST_CASE_METHOD(WrapperFixture, "entityAtDenseIndex + entityFromIndex round-trips to a valid entity", "[ECSWrapper]") {
    EntityID id = ecs.createEntity();
    ecs.addComponent(id, Position{});

    i32 rawIndex = ecs.entityAtDenseIndex<Position>(0); // only one entity in the pool
    EntityID reconstructed = ecs.entityFromIndex(rawIndex);

    REQUIRE(reconstructed.id == id.id);
    REQUIRE(reconstructed.generation == id.generation);
    REQUIRE(ecs.isValidEntity(reconstructed));
}