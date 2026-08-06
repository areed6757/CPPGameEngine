#pragma once
#include <Core/Common.h>

namespace Engine {
	// Deliberately throwaway - no real Faction system exists yet, so team
	// membership just lives inline here as a plain int
	// members ordered largest to smallest, target/resolution are written by DetectionSystem, not AISystem
	struct AIController {
		d64 lastTargetDist{ -1.0 }; // -1 = no sample yet (first tick with this target)
		EntityID target{}; // invalid/default until DetectionSystem resolves a signal lock
		f32 engageRange{ 3.0f }; // stop closing once within this distance, hold and let WeaponSystem fire
		f32 targetResolution{ 0.0f }; // 0 = no lock, 1 = perfect signal lock, degrades weapon aim precision

		// Chase-pulse state: while out of engageRange, if the target is gaining distance
		// (outrunning us) we cut throttle to 0 instead of burning into a losing chase, let
		// MovementDamper bleed off our velocity, then re-engage throttle once we're near-stopped.
		bool throttleHeld{ false }; // true while coasting to a stop before resuming the chase
	};
}