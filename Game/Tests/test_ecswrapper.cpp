#include <catch2/catch_test_macros.hpp>
#include <Game.h>

using namespace Engine;

struct WrapperFixture {
    Logger logger{ Logger::LogLevel::Debug };
    EntityRegister entityReg{ EntityRegisterDesc{ { logger }, 100 } };
    ComponentDesc compDesc{ { logger }, 100 };
    GameECSWrapper ecs{ ECSWrapperDesc{ { logger }, entityReg, compDesc } };
};

TEST_CASE_METHOD(WrapperFixture, "addComponent sets the signature bit and hasComponent reports true", "[ECSWrapper]") {
    EntityID id = ecs.createEntity();
    REQUIRE_FALSE(ecs.hasComponent<Position>(id));

    ecs.addComponent(id, Position{});
    logger.log(Logger::LogLevel::Info, "Entity {} after addComponent<Position>: hasComponent={}, signature={}",
        id.id, ecs.hasComponent<Position>(id), ecs.getSignature(id).to_string());

    REQUIRE(ecs.hasComponent<Position>(id));
    REQUIRE((ecs.getSignature(id) & ecs.makeSignature<Position>()) == ecs.makeSignature<Position>());
}

TEST_CASE_METHOD(WrapperFixture, "removeComponent clears the signature bit", "[ECSWrapper]") {
    EntityID id = ecs.createEntity();
    ecs.addComponent(id, Position{});
    ecs.removeComponent<Position>(id);
    logger.log(Logger::LogLevel::Info, "Entity {} after removeComponent<Position>: hasComponent={}", id.id, ecs.hasComponent<Position>(id));
    REQUIRE_FALSE(ecs.hasComponent<Position>(id));
}

TEST_CASE_METHOD(WrapperFixture, "getComponent returns a mutable reference that persists", "[ECSWrapper]") {
    EntityID id = ecs.createEntity();
    ecs.addComponent(id, Position{});
    ecs.getComponent<Position>(id).rotation = 1.5f;
    logger.log(Logger::LogLevel::Info, "Entity {} rotation set to 1.5, read back as {}", id.id, ecs.getComponent<Position>(id).rotation);
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
    logger.log(Logger::LogLevel::Info, "After removing entity {}'s Position: entity {} rotation={}, entity {} rotation={}, entity {} hasComponent={}",
        b.id, a.id, ecs.getComponent<Position>(a).rotation, c.id, ecs.getComponent<Position>(c).rotation, b.id, ecs.hasComponent<Position>(b));

    REQUIRE_FALSE(ecs.hasComponent<Position>(b));
    REQUIRE(ecs.getComponent<Position>(a).rotation == 1.0f);
    REQUIRE(ecs.getComponent<Position>(c).rotation == 3.0f);
}

TEST_CASE_METHOD(WrapperFixture, "destroyEntity removes all components and invalidates the id", "[ECSWrapper]") {
    EntityID id = ecs.createEntity();
    ecs.addComponent(id, Position{});
    ecs.addComponent(id, Movement{});

    ecs.destroyEntity(id);
    logger.log(Logger::LogLevel::Info, "After destroyEntity({}): isValidEntity={}, signature={}",
        id.id, ecs.isValidEntity(id), ecs.getSignature(id).to_string());

    REQUIRE_FALSE(ecs.isValidEntity(id));
    REQUIRE(ecs.getSignature(id).none()); // no bits left set
}

TEST_CASE_METHOD(WrapperFixture, "entityAtDenseIndex + entityFromIndex round-trips to a valid entity", "[ECSWrapper]") {
    EntityID id = ecs.createEntity();
    ecs.addComponent(id, Position{});

    i32 rawIndex = ecs.entityAtDenseIndex<Position>(0); // only one entity in the pool
    EntityID reconstructed = ecs.entityFromIndex(rawIndex);
    logger.log(Logger::LogLevel::Info, "Original entity (id={}, gen={}) -> dense index {} -> reconstructed (id={}, gen={})",
        id.id, id.generation, rawIndex, reconstructed.id, reconstructed.generation);

    REQUIRE(reconstructed.id == id.id);
    REQUIRE(reconstructed.generation == id.generation);
    REQUIRE(ecs.isValidEntity(reconstructed));
}
