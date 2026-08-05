// ShipCollisionTest.h
#pragma once
#include <Game.h>
#include <random>

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

		void spawnTwoSidedBattle(i32 shipsPerSide, d64 sideSpacing, d64 shipSpacing,
			f32 cooldown, f32 projectileSpeed, f32 projectileRadius, f32 projectileDamage,
			f32 engineThrust, f32 engineMaxAccel);

		// Two mixed fleets (10 small fighters, 6 medium fighters, 2 frigates, 2 destroyers each
		// side) facing off. Small fighters: single engine, fast-firing/weak/short-range guns,
		// quick and close-brawling. Medium fighters: the same archetype scaled up a notch.
		// Frigates: 4 size-1 hardpoints, slow-firing/hard-hitting/long-range guns, and hold that
		// range instead of closing to brawling distance. Destroyers: ~90 parts, 3 engines, 8
		// hardpoints, heaviest/longest-range guns and the biggest icon LOD tier. Exercises
		// weapon/engine variant variety and the full ship-size icon LOD tier set in one scene.
		void spawnTieredFleetBattle(d64 sideSpacing, d64 shipSpacing);

		// TEMP: one-off stress test to see if a 500-part ship bakes/renders/collides without
		// falling over. Remove once confirmed working.
		void spawnMassiveShipTest(Vector2double pos);

	private:
		EntityID buildSmallShip(Vector2double pos, f32 rotation);

		EntityID buildComplexShip(Vector2double pos, f32 rotation, std::mt19937& rng,
			i32 basePartCount, i32 hardpointCount, i32 engineCount, PartVariantID hardpointVariant, PartVariantID weaponVariant, PartVariantID engineVariant,
			bool fixedForwardHardpoints = false);

		PartVariantID m_hullVariant, m_armorVariant, m_hardpointVariant, m_engineVariant;

		GameECSWrapper& m_ecs;
		ShipFactory& m_shipFactory;
		PartRegistry& m_registry;
	};
}