#include <Test/RenderGridTest.h>
#include <Components/Position.h>
#include <Components/Renderable.h>
#include <Graphics/MeshID.h>
#include <chrono>
#include <format>
#include <cmath>

namespace Engine {
    RenderGridTest::RenderGridTest(const RenderGridTestDesc& desc) : Base(desc.base), m_ecs(desc.ecs) {}
    RenderGridTest::~RenderGridTest() {}

    void RenderGridTest::spawnGrid(i32 width, i32 height, d64 spacing) {
        auto start = std::chrono::high_resolution_clock::now();

        m_entities.reserve(static_cast<size_t>(width) * height);
        m_basePositions.reserve(static_cast<size_t>(width) * height);
        m_amplitude = spacing / 2.0; // "half the distance between them"

        d64 halfW = (width - 1) * spacing / 2.0;
        d64 halfH = (height - 1) * spacing / 2.0;

        for (i32 y = 0; y < height; ++y) {
            for (i32 x = 0; x < width; ++x) {
                Vector2double base{ x * spacing -halfW, y * spacing -halfH };

                EntityID id = m_ecs.createEntity();
                m_ecs.addComponent(id, Position{
                    .transform = base,
                    .rotation = 0.0f
                    });
                m_ecs.addComponent(id, Renderable{
                    .mesh = MeshID::Quad,
                    .texture = std::nullopt,
                    .scale = static_cast<f32>(GRID_CELL_SIZE_KM * 5.0)
                    });
                m_ecs.addComponent(id, Physics{ .radius = 0.05f });

                m_entities.push_back(id);
                m_basePositions.push_back(base);
            }
        }

        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now() - start).count();
        EngineLogInfo(std::format("Spawned {}x{} renderable grid ({} entities) in {}ms",
            width, height, width * height, ms).c_str());
    }

    // Traces a diamond (rotated square) of the given amplitude around the
    // origin: top -> right -> bottom -> left -> top, each edge a straight
    // linear interpolation. t is unbounded elapsed time; wraps via fmod.
    Vector2double RenderGridTest::diamondOffset(d64 t) const noexcept {
        d64 cycle = std::fmod(t, m_period) / m_period; // 0..1 over one full loop
        d64 seg = cycle * 4.0;                          // 0..4, one unit per edge
        i32 segIndex = static_cast<i32>(seg);
        d64 frac = seg - static_cast<d64>(segIndex);     // 0..1 within this edge

        Vector2double p0{};
        Vector2double p1{};
        switch (segIndex) {
        case 0: p0 = { 0.0, m_amplitude };  p1 = { m_amplitude, 0.0 };  break;
        case 1: p0 = { m_amplitude, 0.0 };  p1 = { 0.0, -m_amplitude }; break;
        case 2: p0 = { 0.0, -m_amplitude }; p1 = { -m_amplitude, 0.0 }; break;
        default: p0 = { -m_amplitude, 0.0 }; p1 = { 0.0, m_amplitude }; break;
        }

        return Vector2double{
            p0.x + (p1.x - p0.x) * frac,
            p0.y + (p1.y - p0.y) * frac
        };
    }

    void RenderGridTest::Update(d64 dt) {
        m_elapsed += dt;

        for (size_t i = 0; i < m_entities.size(); ++i) {
            EntityID id = m_entities[i];
            if (!m_ecs.isValidEntity(id)) { continue; }

            auto& tform = m_ecs.getComponent<Position>(id);
            Vector2double offset = diamondOffset(m_elapsed);
            tform.transform = m_basePositions[i] + offset;
        }
    }

    void RenderGridTest::spawnProjectiles(i32 count, d64 startDistance, f32 speed, f32 physicsRadius) {
        for (i32 i = 0; i < count; ++i) {
            d64 angle = (2.0 * 3.14159265358979 * i) / static_cast<d64>(count);
            Vector2double startPos{
                std::cos(angle) * startDistance,
                std::sin(angle) * startDistance
            };

            // velocity points from spawn position back toward the origin (grid center)
            Vector2double dirD = Vector2double{ 0.0, 0.0 } - startPos;
            dirD.normalize();
            Vector2float dir{ static_cast<f32>(dirD.x), static_cast<f32>(dirD.y) };

            EntityID id = m_ecs.createEntity();
            m_ecs.addComponent(id, Position{
                .transform = startPos,
                .rotation = 0.0f
                });
            m_ecs.addComponent(id, Movement{
                .linearVelocity = dir * speed,
                .angularVelocity = 0.0f,
                .linearAcceleration = Vector2float{},
                .angularAcceleration = 0.0f
                });
            m_ecs.addComponent(id, Renderable{
                .mesh = MeshID::Quad,
                .texture = std::nullopt,
                .scale = static_cast<f32>(GRID_CELL_SIZE_KM * 5.0)
                });
            m_ecs.addComponent(id, Physics{
                .radius = physicsRadius
                });
        }

        EngineLogInfo(std::format("Spawned {} projectile(s) at distance {} moving toward origin at speed {}",
            count, startDistance, speed).c_str());
    }
}