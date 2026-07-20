#pragma once
#include <Core/Common.h>
#include <GameECS.h>

namespace Engine {
    struct RenderGridTestDesc {
        BaseDesc base;
        GameECSWrapper& ecs;
    };

    class RenderGridTest : public Base {
    public:
        explicit RenderGridTest(const RenderGridTestDesc& desc);
        void spawnGrid(i32 width, i32 height, d64 spacing);
    private:
        GameECSWrapper& m_ecs;
    };
}