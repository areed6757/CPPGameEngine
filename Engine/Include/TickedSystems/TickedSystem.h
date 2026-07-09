#pragma once
#include <Core/Common.h>

// Base class for all systems that are updated per-tick.
// DeltaTime is used to determine actual changes made per tick-call to Update()
namespace Engine {
	class TickedSystem {
	public:
		virtual void Update(d64 deltaTime) = 0;
		virtual ~TickedSystem() = default;
	};
}