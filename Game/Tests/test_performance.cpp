#include <catch2/catch_test_macros.hpp>
#include <chrono>
#include <Core/Logger.h>
#include <ECS/EntityRegister.h>
#include <GameDescs.h>
#include <Components/Position.h>

using namespace Engine;

TEST_CASE("one million entity creation completes within a reasonable time budget", "[performance]") {
    Logger logger{ Logger::LogLevel::Info };
    EntityRegister entityReg{ EntityRegisterDesc{ { logger }, 1'000'000 } };
    ComponentDesc compDesc{ { logger }, 1'000'000 };
    GameECSWrapper ecs{ ECSWrapperDesc{ { logger }, entityReg, compDesc } };

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 1'000'000; ++i) {
        EntityID id = ecs.createEntity();
        ecs.addComponent(id, Position{});
    }
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now() - start).count();

    INFO("Took " << ms << "ms");
    REQUIRE(ms < 1000); // Catches egregious performance faults
}