#pragma once
#include <Game.h>
#include <bitset>

namespace Engine {
    struct QuadtreeDebugSystemDesc {
        BaseDesc base;
        QuadTree& quadtree;
        Camera& camera;
        Window& window;
        DebugLineRenderer& debugLineRenderer;
    };

    class QuadtreeDebugSystem : public Base, public TickedSystem {
    public:
        explicit QuadtreeDebugSystem(const QuadtreeDebugSystemDesc& desc);
        void Update(d64 dt) override;

    private:
        QuadTree& m_quadtree;
        Camera& m_camera;
        Window& m_window;
        DebugLineRenderer& m_debugLineRenderer;
        std::vector<Vector2double> m_boundVerts;
        std::vector<f32> m_flatVerts;
    };
}