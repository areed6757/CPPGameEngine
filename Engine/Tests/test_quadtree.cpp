#include <catch2/catch_test_macros.hpp>
#include <Utilities/QuadTree.h>
#include <Core/Logger.h>
#include <algorithm>

using namespace Engine;

namespace {
    struct QuadTreeFixture {
        Logger logger{ Logger::LogLevel::Info };
        // Small, deliberately tight bounds with a low per-node capacity so a handful of
        // entities is enough to force real subdivision, rather than needing thousands
        // of insertions to exercise QuadTree's subdivide()/childFor() logic.
        QuadTree tree{ QuadTreeDesc{ {logger}, Vector2double{-100.0, -100.0}, Vector2double{100.0, 100.0}, 4, 2 } };
    };

    bool contains(const std::vector<EntityID>& results, i32 id) {
        return std::any_of(results.begin(), results.end(), [id](const EntityID& e) { return e.id == id; });
    }
}

TEST_CASE_METHOD(QuadTreeFixture, "queryAABB on an empty tree returns no results", "[QuadTree]") {
    std::vector<EntityID> results;
    tree.queryAABB({ -100.0, -100.0 }, { 100.0, 100.0 }, results);
    logger.log(Logger::LogLevel::Info, "Empty-tree queryAABB returned {} results", results.size());
    REQUIRE(results.empty());
}

TEST_CASE_METHOD(QuadTreeFixture, "queryAABB covering the whole tree returns every inserted entity", "[QuadTree]") {
    tree.insert(EntityID{ 1, 0 }, { -50.0, -50.0 });
    tree.insert(EntityID{ 2, 0 }, { 50.0, 50.0 });
    tree.insert(EntityID{ 3, 0 }, { -50.0, 50.0 });
    tree.insert(EntityID{ 4, 0 }, { 50.0, -50.0 });

    std::vector<EntityID> results;
    tree.queryAABB({ -100.0, -100.0 }, { 100.0, 100.0 }, results);
    logger.log(Logger::LogLevel::Info, "Full-bounds queryAABB found {} of 4 inserted entities", results.size());

    REQUIRE(results.size() == 4);
    for (i32 id : { 1, 2, 3, 4 }) { REQUIRE(contains(results, id)); }
}

TEST_CASE_METHOD(QuadTreeFixture, "queryAABB restricted to one quadrant excludes entities in other quadrants", "[QuadTree]") {
    tree.insert(EntityID{ 1, 0 }, { -50.0, -50.0 }); // bottom-left quadrant
    tree.insert(EntityID{ 2, 0 }, { 50.0, 50.0 });   // top-right quadrant

    std::vector<EntityID> results;
    tree.queryAABB({ -100.0, -100.0 }, { 0.0, 0.0 }, results); // only the bottom-left quadrant
    logger.log(Logger::LogLevel::Info, "Bottom-left-quadrant queryAABB found {} results (expected just entity 1)", results.size());

    REQUIRE(results.size() == 1);
    REQUIRE(contains(results, 1));
    REQUIRE_FALSE(contains(results, 2));
}

TEST_CASE_METHOD(QuadTreeFixture, "queryRadius returns only entities within the given distance of the center", "[QuadTree]") {
    tree.insert(EntityID{ 1, 0 }, { 0.0, 0.0 });  // distance 0
    tree.insert(EntityID{ 2, 0 }, { 5.0, 0.0 });  // distance 5
    tree.insert(EntityID{ 3, 0 }, { 90.0, 90.0 }); // far away

    std::vector<EntityID> results;
    tree.queryRadius({ 0.0, 0.0 }, 10.0, results);
    logger.log(Logger::LogLevel::Info, "queryRadius(center=(0,0), r=10) found {} results, expected entities 1 and 2", results.size());

    REQUIRE(results.size() == 2);
    REQUIRE(contains(results, 1));
    REQUIRE(contains(results, 2));
    REQUIRE_FALSE(contains(results, 3));
}

TEST_CASE_METHOD(QuadTreeFixture, "inserting past maxEntitiesPerNode subdivides the tree without losing entities", "[QuadTree]") {
    // maxEntitiesPerNode is 2 in this fixture; insert well past that into the same
    // quadrant to force at least one real subdivision, then confirm every entity is
    // still discoverable afterward - this is the regression test for subdivide()/childFor().
    for (i32 i = 1; i <= 10; ++i) {
        tree.insert(EntityID{ i, 0 }, { 10.0 + i, 10.0 + i });
    }

    std::vector<EntityID> results;
    tree.queryAABB({ -100.0, -100.0 }, { 100.0, 100.0 }, results);
    logger.log(Logger::LogLevel::Info, "After 10 insertions past per-node capacity, queryAABB found {} entities", results.size());

    REQUIRE(results.size() == 10);
    for (i32 i = 1; i <= 10; ++i) { REQUIRE(contains(results, i)); }
}

TEST_CASE_METHOD(QuadTreeFixture, "clear removes all previously inserted entities", "[QuadTree]") {
    tree.insert(EntityID{ 1, 0 }, { 0.0, 0.0 });
    tree.insert(EntityID{ 2, 0 }, { 50.0, 50.0 });

    tree.clear();

    std::vector<EntityID> results;
    tree.queryAABB({ -100.0, -100.0 }, { 100.0, 100.0 }, results);
    logger.log(Logger::LogLevel::Info, "queryAABB after clear() found {} entities (expected 0)", results.size());
    REQUIRE(results.empty());
}

TEST_CASE_METHOD(QuadTreeFixture, "clear followed by fresh inserts behaves like a newly-constructed tree", "[QuadTree]") {
    tree.insert(EntityID{ 1, 0 }, { 0.0, 0.0 });
    tree.clear();
    tree.insert(EntityID{ 2, 0 }, { 25.0, 25.0 });

    std::vector<EntityID> results;
    tree.queryAABB({ -100.0, -100.0 }, { 100.0, 100.0 }, results);
    logger.log(Logger::LogLevel::Info, "queryAABB after clear()+reinsert found {} entities (expected only entity 2)", results.size());

    REQUIRE(results.size() == 1);
    REQUIRE(contains(results, 2));
}
