#pragma once
#include <Core/Common.h>
#include <ECSWrapper.h>

namespace Engine {
    class EntityStressTest : public Base {
    public:
        explicit EntityStressTest(const EntityStressTestDesc& desc)
            : Base(desc.base), m_ecs(desc.ecsWrapper) {
        }
        
        void runMillionEntityTest();
        void runChurnTest();

    private:
        ECSWrapper& m_ecs;
    };
}