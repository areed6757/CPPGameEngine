#pragma once
#include <Core/Common.h>
#include <Graphics/DebugLineRenderer.h>
#include <ECS/TickedSystem.h>
#include <Utilities/QuadTree.h>
#include <Graphics/Camera.h>
#include <Graphics/Window.h>
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

        std::bitset<64> getReadSignature() const noexcept { return m_reads; }
        std::bitset<64> getWriteSignature() const noexcept { return m_writes; }

    protected:
        std::bitset<64> m_reads{};
        std::bitset<64> m_writes{};

    private:
        QuadTree& m_quadtree;
        Camera& m_camera;
        Window& m_window;
        DebugLineRenderer& m_debugLineRenderer;
        std::vector<Vector2double> m_boundVerts;
        std::vector<f32> m_flatVerts;
    };
}