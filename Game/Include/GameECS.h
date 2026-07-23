#pragma once
#include <ECS/ECSWrapper.h>
#include <Utilities/CommandBuffer.h>
#include <Components/ComponentPools.h>

namespace Engine {
	using GameECSWrapper = ECSWrapper<ComponentPools>;
	using GameCommandBuffer = CommandBuffer<ComponentPools>;
	using GameCommandBufferDesc = CommandBufferDesc<ComponentPools>;
}