#pragma once
#include <ECS/ECSWrapper.h>
#include <type_traits>

// Engine has no component/pools type of its own (ComponentPools lives in the Game module),
// so a minimal test-only pools type lives here, mirroring Game/Include/Components/ComponentPools.h,
// shared by any Engine test that needs a concrete ECSWrapper<TPools> or CommandBuffer<TPools>.
namespace Engine {
    struct TestPosition {
        d64 x{ 0.0 };
        d64 y{ 0.0 };
    };

    struct TestVelocity {
        d64 dx{ 0.0 };
        d64 dy{ 0.0 };
    };

    template <> struct ComponentBit<TestPosition> { static constexpr i32 value = 0; };
    template <> struct ComponentBit<TestVelocity> { static constexpr i32 value = 1; };

    struct TestPools {
        explicit TestPools(const ComponentDesc& desc) : positions(desc), velocities(desc) {}

        template <typename T>
        Component<T>& getPool() {
            if constexpr (std::is_same_v<T, TestPosition>) return positions;
            else if constexpr (std::is_same_v<T, TestVelocity>) return velocities;
            else static_assert(sizeof(T) == 0, "getPool: unregistered component type");
        }

        template <typename T>
        const Component<T>& getPool() const {
            if constexpr (std::is_same_v<T, TestPosition>) return positions;
            else if constexpr (std::is_same_v<T, TestVelocity>) return velocities;
            else static_assert(sizeof(T) == 0, "getPool: unregistered component type");
        }

        void removeAll(i32 index) {
            if (positions.has(index)) { positions.remove(index); }
            if (velocities.has(index)) { velocities.remove(index); }
        }

        Component<TestPosition> positions;
        Component<TestVelocity> velocities;
    };

    using TestECSWrapper = ECSWrapper<TestPools>;
}
