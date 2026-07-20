#pragma once
#include <Core/Common.h>
#include <GameECS.h>
#include <ECS/TickedSystem.h>
#include <Physics/Vector2double.h>
#include <vector>

namespace Engine {
    struct RenderGridTestDesc {
        BaseDesc base;
        GameECSWrapper& ecs;
    };

    // Test/stress utility: spawns a WxH grid of renderable entities and moves
    // each one in a small diamond path around its own spawn position, to
    // exercise ECS + Position + Renderable + RenderSystem + Camera together
    // under real per-tick movement rather than a static grid.
    class RenderGridTest : public Base, public TickedSystem {
    public:
        explicit RenderGridTest(const RenderGridTestDesc& desc);
        ~RenderGridTest();

        // Spawns width*height entities on a grid with the given spacing (km).
        // Each entity's own spawn position becomes the center of its diamond
        // path; diamond "radius" (center to vertex) is spacing / 2.
        void spawnGrid(i32 width, i32 height, d64 spacing);

        // Advances every spawned entity's position along its diamond path.
        // Register via Scheduler::registerFrameSystem (visual-only motion,
        // no gameplay meaning) or registerSystem if fixed-tick determinism
        // is preferred for this test instead.
        void Update(d64 dt) override;

    private:
        [[nodiscard]] Vector2double diamondOffset(d64 t) const noexcept;

        GameECSWrapper& m_ecs;

        std::vector<EntityID> m_entities;
        std::vector<Vector2double> m_basePositions;

        d64 m_amplitude{};   // spacing / 2, set in spawnGrid
        d64 m_period{ 4.0 }; // seconds per full diamond loop — tune by feel
        d64 m_elapsed{};
    };
}