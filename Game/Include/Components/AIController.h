#pragma once
#include <Core/Common.h>

namespace Engine {
	// Deliberately throwaway - no real Faction system exists yet, so team
	// membership just lives inline here as a plain int
	struct AIController {
		EntityID target{}; // invalid/default until a target is found
		f32 engageRange{ 3.0f }; // stop closing once within this distance, hold and let WeaponSystem fire
	};
}