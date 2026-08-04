#include <catch2/catch_test_macros.hpp>
#include <Physics/AABBTree.h>
#include <Core/Logger.h>
#include <algorithm>

using namespace Engine;

namespace {
    struct AABBTreeFixture {
        Logger logger{ Logger::LogLevel::Info };
    };

    bool containsEntity(const std::vector<EntityID>& results, i32 id) {
        return std::any_of(results.begin(), results.end(), [&](const EntityID& e) { return e.id == id; });
    }
}

TEST_CASE_METHOD(AABBTreeFixture, "insert fattens the stored AABB by fatMargin in every direction", "[AABBTree]") {
    AABBTree tree{ AABBTreeDesc{ {logger}, 0.5f, 1.0f } };

    AABB bounds{ Vector2double{ 0.0, 0.0 }, Vector2double{ 2.0, 2.0 } };
    i32 proxy = tree.insert(EntityID{ 1, 0 }, bounds);

    const AABB& fat = tree.getFatAABB(proxy);
    logger.log(Logger::LogLevel::Info, "Inserted bounds ({},{})-({},{}), fattened to ({},{})-({},{})",
        bounds.min.x, bounds.min.y, bounds.max.x, bounds.max.y, fat.min.x, fat.min.y, fat.max.x, fat.max.y);

    REQUIRE(fat.min.x == -0.5);
    REQUIRE(fat.min.y == -0.5);
    REQUIRE(fat.max.x == 2.5);
    REQUIRE(fat.max.y == 2.5);
    REQUIRE(tree.getEntity(proxy).id == 1);
}

TEST_CASE_METHOD(AABBTreeFixture, "query finds an inserted entity whose AABB overlaps the query bounds", "[AABBTree]") {
    AABBTree tree{ AABBTreeDesc{ {logger}, 0.0f, 1.0f } };
    tree.insert(EntityID{ 1, 0 }, AABB{ Vector2double{ 0.0, 0.0 }, Vector2double{ 1.0, 1.0 } });

    std::vector<EntityID> results;
    tree.query(AABB{ Vector2double{ 0.5, 0.5 }, Vector2double{ 1.5, 1.5 } }, results);

    REQUIRE(results.size() == 1);
    REQUIRE(containsEntity(results, 1));
}

TEST_CASE_METHOD(AABBTreeFixture, "query returns nothing for bounds that overlap no inserted entity", "[AABBTree]") {
    AABBTree tree{ AABBTreeDesc{ {logger}, 0.0f, 1.0f } };
    tree.insert(EntityID{ 1, 0 }, AABB{ Vector2double{ 0.0, 0.0 }, Vector2double{ 1.0, 1.0 } });

    std::vector<EntityID> results;
    tree.query(AABB{ Vector2double{ 100.0, 100.0 }, Vector2double{ 101.0, 101.0 } }, results);

    REQUIRE(results.empty());
}

TEST_CASE_METHOD(AABBTreeFixture, "query returns every overlapping entity and excludes non-overlapping ones", "[AABBTree]") {
    AABBTree tree{ AABBTreeDesc{ {logger}, 0.0f, 1.0f } };
    tree.insert(EntityID{ 1, 0 }, AABB{ Vector2double{ 0.0, 0.0 }, Vector2double{ 1.0, 1.0 } });
    tree.insert(EntityID{ 2, 0 }, AABB{ Vector2double{ 0.5, 0.5 }, Vector2double{ 1.5, 1.5 } });
    tree.insert(EntityID{ 3, 0 }, AABB{ Vector2double{ 50.0, 50.0 }, Vector2double{ 51.0, 51.0 } });

    std::vector<EntityID> results;
    tree.query(AABB{ Vector2double{ 0.0, 0.0 }, Vector2double{ 1.5, 1.5 } }, results);

    logger.log(Logger::LogLevel::Info, "Query over the overlapping pair's region returned {} entities", results.size());
    REQUIRE(results.size() == 2);
    REQUIRE(containsEntity(results, 1));
    REQUIRE(containsEntity(results, 2));
    REQUIRE_FALSE(containsEntity(results, 3));
}

TEST_CASE_METHOD(AABBTreeFixture, "remove erases the entity from future queries", "[AABBTree]") {
    AABBTree tree{ AABBTreeDesc{ {logger}, 0.0f, 1.0f } };
    i32 proxy = tree.insert(EntityID{ 1, 0 }, AABB{ Vector2double{ 0.0, 0.0 }, Vector2double{ 1.0, 1.0 } });

    tree.remove(proxy);

    std::vector<EntityID> results;
    tree.query(AABB{ Vector2double{ 0.0, 0.0 }, Vector2double{ 1.0, 1.0 } }, results);
    REQUIRE(results.empty());
}

TEST_CASE_METHOD(AABBTreeFixture, "moveProxy is a no-op when the new bounds are still inside the existing fat AABB", "[AABBTree]") {
    AABBTree tree{ AABBTreeDesc{ {logger}, 0.5f, 1.0f } };
    i32 proxy = tree.insert(EntityID{ 1, 0 }, AABB{ Vector2double{ 0.0, 0.0 }, Vector2double{ 2.0, 2.0 } });
    AABB fatBefore = tree.getFatAABB(proxy);

    bool moved = tree.moveProxy(proxy, AABB{ Vector2double{ 0.2, 0.2 }, Vector2double{ 2.0, 2.0 } }, Vector2double{ 0.0, 0.0 });

    logger.log(Logger::LogLevel::Info, "moveProxy with bounds still inside the fat AABB -> real update happened: {}", moved);
    REQUIRE_FALSE(moved);
    REQUIRE(tree.getFatAABB(proxy).min.x == fatBefore.min.x);
    REQUIRE(tree.getFatAABB(proxy).max.x == fatBefore.max.x);
}

TEST_CASE_METHOD(AABBTreeFixture, "moveProxy relocates the entity once the new bounds escape the fat AABB", "[AABBTree]") {
    AABBTree tree{ AABBTreeDesc{ {logger}, 0.5f, 1.0f } };
    i32 proxy = tree.insert(EntityID{ 1, 0 }, AABB{ Vector2double{ 0.0, 0.0 }, Vector2double{ 2.0, 2.0 } });

    bool moved = tree.moveProxy(proxy, AABB{ Vector2double{ 10.0, 10.0 }, Vector2double{ 12.0, 12.0 } }, Vector2double{ 0.0, 0.0 });
    REQUIRE(moved);

    std::vector<EntityID> atOldPosition;
    tree.query(AABB{ Vector2double{ -1.0, -1.0 }, Vector2double{ 3.0, 3.0 } }, atOldPosition);
    REQUIRE(atOldPosition.empty());

    std::vector<EntityID> atNewPosition;
    tree.query(AABB{ Vector2double{ 9.0, 9.0 }, Vector2double{ 13.0, 13.0 } }, atNewPosition);
    REQUIRE(containsEntity(atNewPosition, 1));
}

TEST_CASE_METHOD(AABBTreeFixture, "the tree stays correct once it outgrows its initial node pool capacity", "[AABBTree]") {
    AABBTree tree{ AABBTreeDesc{ {logger}, 0.0f, 1.0f } };
    constexpr i32 entityCount = 200; // initial node pool is 16 nodes, this forces multiple growths

    for (i32 i = 0; i < entityCount; i++) {
        d64 x = static_cast<d64>(i) * 2.0;
        tree.insert(EntityID{ i, 0 }, AABB{ Vector2double{ x, 0.0 }, Vector2double{ x + 1.0, 1.0 } });
    }

    std::vector<EntityID> results;
    tree.query(AABB{ Vector2double{ 0.0, 0.0 }, Vector2double{ static_cast<d64>(entityCount) * 2.0, 1.0 } }, results);

    logger.log(Logger::LogLevel::Info, "Inserted {} entities across multiple pool growths, query over the full span found {}", entityCount, results.size());
    REQUIRE(results.size() == entityCount);
    for (i32 i = 0; i < entityCount; i++) { REQUIRE(containsEntity(results, i)); }
}
