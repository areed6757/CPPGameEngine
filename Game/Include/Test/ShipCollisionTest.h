// ShipCollisionTest.h
#pragma once
#include <Core/Base.h>
#include <Core/Common.h>
#include <GameECS.h>
#include <Ships/ShipFactory.h>
#include <Ships/PartRegistry.h>

namespace Engine {
	struct ShipCollisionTestDesc {
		BaseDesc base;
		GameECSWrapper& ecs;
		ShipFactory& shipFactory;
		PartRegistry& partRegistry;
	};

	// Exercises the new narrow-phase pipeline in isolation from any other
	// test scene: one pair of overlapping baked ships (exercises SAT
	// prefilter + boundaryEdgesIntersect, should produce a real CollisionEvent
	// with a plausible hitPoint), one pair of clearly-separated ships (should
	// produce no event at all -- confirms the SAT prefilter's cheap reject
	// path isn't silently reporting false positives), and one projectile
	// fired directly at a stationary ship (exercises raycastGrid).
	class ShipCollisionTest : public Base {
	public:
		explicit ShipCollisionTest(const ShipCollisionTestDesc& desc);
		~ShipCollisionTest();

		// Two small ships built from the same simple grid, placed close
		// enough that their hulls genuinely overlap.
		void spawnOverlappingShipPair();

		// Same two ships, placed far enough apart that broad phase (or at
		// worst the SAT prefilter) should reject the pair outright.
		void spawnSeparatedShipPair();

		// One stationary ship, one projectile launched directly at it from
		// outside its hull -- should register a projectile-vs-ship hit via
		// raycastGrid, not the generic swept-circle fallback.
		void spawnProjectileAtShip(f32 projectileSpeed, f32 projectileDamage);

	private:
		EntityID buildSmallShip(Vector2double pos, f32 rotation);

		GameECSWrapper& m_ecs;
		ShipFactory& m_shipFactory;
		PartRegistry& m_registry;
		PartVariantID m_hullVariant{ INVALID_PART_VARIANT };
	};
}