#include <Test/RenderGridTest.h>
#include <Components/Position.h>
#include <Components/Renderable.h>
#include <Graphics/MeshID.h>
#include <chrono>
#include <format>

namespace Engine {
    RenderGridTest::RenderGridTest(const RenderGridTestDesc& desc) : Base(desc.base), m_ecs(desc.ecs) {}

    void RenderGridTest::spawnGrid(i32 width, i32 height, d64 spacing) {
        auto start = std::chrono::high_resolution_clock::now();

        for (i32 y = 0; y < height; ++y) {
            for (i32 x = 0; x < width; ++x) {
                EntityID id = m_ecs.createEntity();
                m_ecs.addComponent(id, Position{
                    .transform = Vector2double{ x * spacing, y * spacing },
                    .rotation = 0.0f
                    });
                m_ecs.addComponent(id, Renderable{
                    .mesh = MeshID::Quad,
                    .texture = std::nullopt
                    });
            }
        }

        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now() - start).count();
        EngineLogInfo(std::format("Spawned {}x{} renderable grid ({} entities) in {}ms",
            width, height, width * height, ms).c_str());
    }
}