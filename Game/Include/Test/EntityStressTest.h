#pragma once
#include <Core/Common.h>
#include <GameDescs.h>
#include <GameECS.h>

namespace Engine {
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