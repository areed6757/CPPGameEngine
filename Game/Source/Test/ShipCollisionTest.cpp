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
		i32 hardpointCount, PartVariantID hardpointVariant, PartVariantID weaponVariant, PartVariantID engineVariant)
	{
		constexpr i32 GRID_SIZE = 7;
		i32 targetParts = 37 + hardpointCount; // base hull/armor growth + N hardpoints + 1 engine
		constexpr i32 DX[4] = { 1, -1, 0, 0 };
		constexpr i32 DY[4] = { 0, 0, 1, -1 };

		ShipGridDesc sgDesc{ {m_logger}, GRID_SIZE, GRID_SIZE };
		ShipGrid grid(sgDesc);

		std::vector<std::pair<i32, i32>> placed;
		i32 seed = GRID_SIZE / 2;
		grid.tryPlacePart(seed, seed, 1, 1, PartCategory::Hull, m_hullVariant);
		placed.emplace_back(seed, seed);

		std::uniform_int_distribution<i32> pickDir(0, 3);
		std::uniform_int_distribution<i32> pickPart(0, 4);

		std::vector<std::pair<i32, i32>> hardpointPositions;
		std::pair<i32, i32> enginePos{ -1, -1 };
		i32 attempts = 0;

		while (static_cast<i32>(placed.size()) < targetParts && attempts < targetParts * 8) {
			++attempts;

			std::uniform_int_distribution<size_t> pickPlaced(0, placed.size() - 1);
			auto [fromX, fromY] = placed[pickPlaced(rng)];
			i32 dir = pickDir(rng);
			i32 nx = fromX + DX[dir], ny = fromY + DY[dir];

			// Reserve the last (hardpointCount + 1) growth slots: hardpointCount
			// of them for hardpoints, the final one for the engine. Same
			// "guaranteed, not left to chance" reservation as before, just
			// generalized to N hardpoints instead of exactly one.
			i32 remaining = targetParts - static_cast<i32>(placed.size());
			bool needsEngine = enginePos.first == -1;
			bool needsHardpoint = static_cast<i32>(hardpointPositions.size()) < hardpointCount;

			if (needsHardpoint && remaining <= hardpointCount + 1) {
				if (grid.tryPlacePart(nx, ny, 1, 1, PartCategory::Hardpoint, hardpointVariant)) {
					hardpointPositions.emplace_back(nx, ny);
					placed.emplace_back(nx, ny);
				}
				continue;
			}
			if (needsEngine && remaining <= 1) {
				if (grid.tryPlacePart(nx, ny, 1, 1, PartCategory::Engine, engineVariant)) {
					enginePos = { nx, ny };
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
		for (auto& hp : hardpointPositions) { loadout[hp] = weaponVariant; }

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

	void ShipCollisionTest::spawnTwoSidedBattle(i32 shipsPerSide, d64 sideSpacing, d64 shipSpacing,
		f32 cooldown, f32 projectileSpeed, f32 projectileRadius, f32 projectileDamage,
		f32 engineThrust, f32 engineMaxAccel)
	{
		PartVariantID weaponVariant = m_registry.registerVariant(PartVariant{
			.name = "Battle Weapon", .category = PartCategory::Weapon,
			.params = WeaponParams{ PartBaseStats{ 1.0f, 10.0f, 2.0f, 0.0f }, projectileDamage, cooldown, projectileSpeed, projectileRadius }
			});
		m_engineVariant = m_registry.registerVariant(PartVariant{
			.name = "Battle Engine", .category = PartCategory::Engine,
			.params = EngineParams{ PartBaseStats{ 3.0f, 15.0f, 3.0f, 0.0f }, engineThrust, engineMaxAccel }
			});

		std::mt19937 rng{ std::random_device{}() };
		i32 side = static_cast<i32>(std::ceil(std::sqrt(static_cast<d64>(shipsPerSide))));
		d64 half = (side - 1) * shipSpacing / 2.0;

		auto spawnSide = [&](d64 xOffset, f32 facing, i32 teamId, i32 count) {
			i32 spawned = 0;
			for (i32 y = 0; y < side && spawned < count; ++y) {
				for (i32 x = 0; x < side && spawned < count; ++x) {
					Vector2double pos{ xOffset + x * shipSpacing - half, y * shipSpacing - half };
					EntityID ship = buildComplexShip(pos, facing, rng, 2, m_hardpointVariant, weaponVariant, m_engineVariant);

					m_ecs.addComponent(ship, Faction{ .teamId = teamId });
					m_ecs.addComponent(ship, Separation{ .margin = 0.1f });
					m_ecs.addComponent(ship, AIController{
						.target = EntityID{},
						.engageRange = 3.0f,
						});

					++spawned;
				}
			}
			};

		// Two groups facing each other, separated by sideSpacing along X --
		// team 0 faces +x (rotation 0), team 1 faces -x (rotation pi).
		spawnSide(-sideSpacing / 2.0, 0.0f, 0, shipsPerSide);
		spawnSide(sideSpacing / 2.0, 3.14159265f, 1, shipsPerSide);

		EngineLogInfo("ShipCollisionTest: spawned two-sided battle ({} ships per side, {} total).", shipsPerSide, shipsPerSide * 2);
	}
}