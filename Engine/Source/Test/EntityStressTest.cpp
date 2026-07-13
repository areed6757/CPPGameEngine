#include <Test/EntityStressTest.h>
#include <ECS/Components/Transform.h>
#include <chrono>

namespace Engine {
    void EntityStressTest::runMillionEntityTest() {
        constexpr i32 count = 1'000'000;
        std::vector<EntityID> ids;
        ids.reserve(count);

        auto start = std::chrono::high_resolution_clock::now();

        for (i32 i = 0; i < count; ++i) {
            EntityID id = m_ecs.createEntity();
            m_ecs.addComponent(id, Transform{ .transform = Vector3double{0.0, 0.0, 0.0}, .rotation = 0.0f });
            ids.push_back(id);
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        EngineLogInfo(std::format("Created {} entities with Transform in {}ms", count, ms).c_str());
    }
}