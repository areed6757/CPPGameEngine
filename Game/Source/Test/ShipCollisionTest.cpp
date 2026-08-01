// ShipCollisionTest.cpp
#include <Test/ShipCollisionTest.h>
#include <Ships/ShipGrid.h>
#include <Components/Position.h>
#include <Components/Movement.h>
#include <Components/Physics.h>
#include <Components/Renderable.h>
#include <Components/DamagePayload.h>
#include <Components/Lifetime.h>
#include <Components/Mount.h>
#include <Components/Weapon.h>
#include <Graphics/MeshID.h>
#include <cmath>
#include <random>

namespace Engine {
	ShipCollisionTest::ShipCollisionTest(const ShipCollisionTestDesc& desc) : Base(desc.base),
		m_ecs(desc.ecs), m_shipFactory(desc.shipFactory), m_registry(desc.partRegistry)
	{
		// One shared, minimal hull variant reused by every test ship built
		// here -- this test only cares about collision geometry, not part
		// variety, so a single 5x5 solid block is deliberately simple.
		m_hullVariant = m_registry.registerVariant(PartVariant{
			.name = "Test Hull", .category = PartCategory::Hull,
			.params = HullParams{ PartBaseStats{ 5.0f, 20.0f, 10.0f, 0.0f } }
			});

		// Shared armor variant reused by every corner plate on complex test
		// ships -- lighter and less tanky than the hull, just enough to be
		// a visually/mechanically distinct second part type.
		m_armorVariant = m_registry.registerVariant(PartVariant{
			.name = "Test Armor", .category = PartCategory::Armor,
			.params = ArmorParams{ PartBaseStats{ 2.0f, 10.0f, 5.0f, 0.0f } }
			});

		m_hardpointVariant = m_registry.registerVariant(PartVariant{
			.name = "Test Hardpoint", .category = PartCategory::Hardpoint,
			.params = HardpointParams{ PartBaseStats{ 1.0f, 10.0f, 2.0f, 0.0f }, 1, 1 }
			});
	}

	ShipCollisionTest::~ShipCollisionTest() {}

	EntityID ShipCollisionTest::buildSmallShip(Vector2double pos, f32 rotation)
	{
		ShipGridDesc sgDesc{ {m_logger}, 5, 5 };
		ShipGrid grid(sgDesc);
		grid.tryPlacePart(0, 0, 5, 5, PartCategory::Hull, m_hullVariant);

		static const std::unordered_map<std::pair<i32, i32>, PartVariantID, PairHash> noHardpoints;
		return m_shipFactory.bake(grid, pos, rotation, noHardpoints);
	}

	EntityID ShipCollisionTest::buildComplexShip(Vector2double pos, f32 rotation, std::mt19937& rng,
		PartVariantID hardpointVariant, PartVariantID weaponVariant)
	{
		constexpr i32 GRID_SIZE = 7;
		constexpr i32 TARGET_PARTS = 8; // includes the hardpoint now, not just hull/armor
		constexpr i32 DX[4] = { 1, -1, 0, 0 };
		constexpr i32 DY[4] = { 0, 0, 1, -1 };

		ShipGridDesc sgDesc{ {m_logger}, GRID_SIZE, GRID_SIZE };
		ShipGrid grid(sgDesc);

		std::vector<std::pair<i32, i32>> placed;
		i32 seed = GRID_SIZE / 2;
		grid.tryPlacePart(seed, seed, 1, 1, PartCategory::Hull, m_hullVariant);
		placed.emplace_back(seed, seed);

		std::uniform_int_distribution<i32> pickDir(0, 3);
		std::uniform_int_distribution<i32> pickPart(0, 4); // 1-in-5 chance of armor instead of hull

		std::pair<i32, i32> hardpointPos{ -1, -1 };
		i32 attempts = 0;
		while (static_cast<i32>(placed.size()) < TARGET_PARTS && attempts < TARGET_PARTS * 8) {
			++attempts;

			std::uniform_int_distribution<size_t> pickPlaced(0, placed.size() - 1);
			auto [fromX, fromY] = placed[pickPlaced(rng)];
			i32 dir = pickDir(rng);
			i32 nx = fromX + DX[dir], ny = fromY + DY[dir];

			// Reserve the last growth slot for the hardpoint, once everything
			// else has grown -- guarantees exactly one hardpoint per ship
			// rather than leaving it to chance alongside hull/armor picks.
			bool placeHardpoint = (hardpointPos.first == -1) && (static_cast<i32>(placed.size()) == TARGET_PARTS - 1);

			if (placeHardpoint) {
				if (grid.tryPlacePart(nx, ny, 1, 1, PartCategory::Hardpoint, hardpointVariant)) {
					hardpointPos = { nx, ny };
					placed.emplace_back(nx, ny);
				}
				continue;
			}

			bool useArmor = pickPart(rng) == 4;
			PartCategory category = useArmor ? PartCategory::Armor : PartCategory::Hull;
			PartVariantID variant = useArmor ? m_armorVariant : m_hullVariant;

			if (grid.tryPlacePart(nx, ny, 1, 1, category, variant)) {
				placed.emplace_back(nx, ny);
			}
		}

		std::unordered_map<std::pair<i32, i32>, PartVariantID, PairHash> loadout;
		if (hardpointPos.first != -1) { loadout[hardpointPos] = weaponVariant; }

		return m_shipFactory.bake(grid, pos, rotation, loadout);
	}

	void ShipCollisionTest::spawnOverlappingShipPair()
	{
		// 5x5 grid at GRID_CELL_SIZE_KM is 0.05km wide -- placing centers
		// 0.03km apart guarantees real hull overlap, not just a near-miss.
		EntityID a = buildSmallShip(Vector2double{ -0.015, 0.0 }, 0.0f);
		EntityID b = buildSmallShip(Vector2double{ 0.015, 0.0 }, 0.0f);

		EngineLogInfo("ShipCollisionTest: spawned overlapping ship pair (entities {}, {}) -- expect a CollisionEvent with a real hitPoint this tick.",
			a.id, b.id);
	}

	void ShipCollisionTest::spawnSeparatedShipPair()
	{
		// Same ship size, placed several ship-widths apart -- broad phase
		// or the SAT prefilter should reject this pair before any real
		// geometry test runs.
		EntityID a = buildSmallShip(Vector2double{ -5.0, 5.0 }, 0.0f);
		EntityID b = buildSmallShip(Vector2double{ 5.0, 5.0 }, 0.0f);

		EngineLogInfo("ShipCollisionTest: spawned separated ship pair (entities {}, {}) -- expect NO CollisionEvent between them.",
			a.id, b.id);
	}

	void ShipCollisionTest::spawnProjectileAtShip(f32 projectileSpeed, f32 projectileDamage)
	{
		EntityID ship = buildSmallShip(Vector2double{ 0.0, -3.0 }, 0.0f);
		EntityID other = buildSmallShip(Vector2double{ 0.0, -6.0 }, 0.0f);

		// Projectile starts well outside the ship's hull, on a direct
		// collision course -- should hit via raycastGrid, not the generic
		// swept-circle fallback (the ship has ShipCollisionGeometry +
		// ShipGridData, so the dispatch in CollisionSystem::Update() should
		// route this to narrowPhaseProjectileVsShip).
		EntityID projectile = m_ecs.createEntity();
		m_ecs.addComponent(projectile, Position{ .transform = Vector2double{ 0.0, -6.0 }, .rotation = 1.5708f });
		m_ecs.addComponent(projectile, Movement{ .linearVelocity = Vector2float{ 0.0f, projectileSpeed } });
		m_ecs.addComponent(projectile, Physics{ .radius = 0.005f, .mass = 0.01f });
		m_ecs.addComponent(projectile, Renderable{ .mesh = MeshID::Quad, .texture = std::nullopt, .scale = 0.01f });
		m_ecs.addComponent(projectile, DamagePayload{ .amount = projectileDamage, .source = other });
		m_ecs.addComponent(projectile, Lifetime{ .remaining = 5.0f });

		EngineLogInfo("ShipCollisionTest: spawned ship {} and projectile {} on a collision course -- expect a projectile-vs-ship CollisionEvent via raycastGrid.",
			ship.id, projectile.id);
	}

	void ShipCollisionTest::spawnMassBattle(i32 shipCount, d64 spacing, f32 cooldown, f32 projectileSpeed, f32 projectileRadius, f32 projectileDamage)
	{
		PartVariantID weaponVariant = m_registry.registerVariant(PartVariant{
			.name = "Test Weapon", .category = PartCategory::Weapon,
			.params = WeaponParams{ PartBaseStats{ 1.0f, 10.0f, 2.0f, 0.0f }, projectileDamage, cooldown, projectileSpeed, 0.005f }
			});

		std::mt19937 rng{ std::random_device{}() };
		std::uniform_real_distribution<f32> angleDist(0.0f, 6.2831853f);
		i32 side = static_cast<i32>(std::ceil(std::sqrt(static_cast<d64>(shipCount))));
		d64 half = (side - 1) * spacing / 2.0;

		i32 spawned = 0;
		for (i32 y = 0; y < side && spawned < shipCount; ++y) {
			for (i32 x = 0; x < side && spawned < shipCount; ++x) {
				Vector2double shipPos{ x * spacing - half, y * spacing - half };
				f32 facing = angleDist(rng);
				buildComplexShip(shipPos, facing, rng, m_hardpointVariant, weaponVariant);
				++spawned;
			}
		}
		EngineLogInfo("ShipCollisionTest: spawned mass battle ({} irregular multi-part ships, each firing via a real grown hardpoint).", spawned);
	}
}