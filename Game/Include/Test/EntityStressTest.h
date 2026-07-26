#pragma once
#include <Engine.h>
#include <GameECS.h>

namespace Engine {
    struct EntityStressTestDesc {
        BaseDesc base;
        GameECSWrapper& ecs;
    };

    class EntityStressTest : public Base {
    public:
        explicit EntityStressTest(const EntityStressTestDesc& desc)
            : Base(desc.base), m_ecs(desc.ecs) {
        }
        
        void runMillionEntityTest();
        void runChurnTest();

    private:
        GameECSWrapper& m_ecs;
    };
}