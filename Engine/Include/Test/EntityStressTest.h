#pragma once
#include <Core/Common.h>
#include <ECS/ECSWrapper.h>

namespace Engine {
    class EntityStressTest : public Base {
    public:
        explicit EntityStressTest(const EntityStressTestDesc& desc)
            : Base(desc.base), m_ecs(desc.ecsWrapper) {
        }

        void runMillionEntityTest();

    private:
        ECSWrapper& m_ecs;
    };
}