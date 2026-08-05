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
		
		m_engineVariant = {};
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
		i32 basePartCount, i32 hardpointCount, i32 engineCount, PartVariantID hardpointVariant, PartVariantID weaponVariant, PartVariantID engineVariant)
	{
		i32 targetParts = basePartCount + hardpointCount + engineCount; // base hull/armor growth + N hardpoints + M engines
		// Grid must comfortably outgrow targetParts -- organic frontier growth doesn't perfectly
		// pack a square (walls force early corner-filling, and stray fully-enclosed holes can
		// form), so pad both proportionally and with a fixed minimum, not just size-to-fit.
		i32 GRID_SIZE = static_cast<i32>(std::ceil(std::sqrt(static_cast<f32>(targetParts) * 1.2f))) + 10;
		constexpr i32 DX[4] = { 1, -1, 0, 0 };
		constexpr i32 DY[4] = { 0, 0, 1, -1 };

		ShipGridDesc sgDesc{ {m_logger}, GRID_SIZE, GRID_SIZE };
		ShipGrid grid(sgDesc);

		auto isCellFree = [&](i32 x, i32 y) {
			return grid.inBounds(x, y) && grid.at(x, y).partCategory == PartCategory::None;
			};
		auto hasOpenNeighbor = [&](i32 x, i32 y) {
			for (i32 d = 0; d < 4; d++) {
				if (isCellFree(x + DX[d], y + DY[d])) { return true; }
			}
			return false;
			};

		std::vector<std::pair<i32, i32>> placed;
		i32 seed = GRID_SIZE / 2;
		grid.tryPlacePart(seed, seed, 1, 1, PartCategory::Hull, m_hullVariant);
		placed.emplace_back(seed, seed);

		std::uniform_int_distribution<i32> pickDir(0, 3);
		std::uniform_int_distribution<i32> pickPart(0, 4);

		std::vector<std::pair<i32, i32>> hardpointPositions;
		std::vector<std::pair<i32, i32>> enginePositions;

		// Frontier: placed cells that still have at least one open neighbor. Growth picks from
		// this instead of all placed cells -- as the ship grows, interior cells (whose neighbors
		// are all filled) come to vastly outnumber the actual growable edge, so sampling
		// uniformly from every placed cell (the old approach) makes each attempt's success rate
		// collapse at large part counts. Sampling only the frontier keeps placement O(1) per
		// part regardless of how big the ship gets.
		std::vector<std::pair<i32, i32>> frontier;
		frontier.emplace_back(seed, seed);

		while (static_cast<i32>(placed.size()) < targetParts && !frontier.empty()) {
			std::uniform_int_distribution<size_t> pickFrontier(0, frontier.size() - 1);
			size_t frontierIdx = pickFrontier(rng);
			auto [fromX, fromY] = frontier[frontierIdx];
			i32 dir = pickDir(rng);
			i32 nx = fromX + DX[dir], ny = fromY + DY[dir];

			if (!isCellFree(nx, ny)) {
				if (!hasOpenNeighbor(fromX, fromY)) {
					frontier[frontierIdx] = frontier.back();
					frontier.pop_back();
				}
				continue;
			}

			// Reserve the last (hardpointCount + engineCount) growth slots: hardpointCount
			// of them for hardpoints, the rest for engines. Same "guaranteed, not left to
			// chance" reservation as before, just generalized to N hardpoints and M engines.
			i32 remaining = targetParts - static_cast<i32>(placed.size());
			bool needsEngine = static_cast<i32>(enginePositions.size()) < engineCount;
			bool needsHardpoint = static_cast<i32>(hardpointPositions.size()) < hardpointCount;

			PartCategory category;
			PartVariantID variant;
			bool isHardpoint = needsHardpoint && remaining <= hardpointCount + engineCount;
			bool isEngine = !isHardpoint && needsEngine && remaining <= engineCount;
			if (isHardpoint) { category = PartCategory::Hardpoint; variant = hardpointVariant; }
			else if (isEngine) { category = PartCategory::Engine; variant = engineVariant; }
			else {
				bool useArmor = pickPart(rng) == 4;
				category = useArmor ? PartCategory::Armor : PartCategory::Hull;
				variant = useArmor ? m_armorVariant : m_hullVariant;
			}

			if (grid.tryPlacePart(nx, ny, 1, 1, category, variant)) {
				placed.emplace_back(nx, ny);
				if (isHardpoint) { hardpointPositions.emplace_back(nx, ny); }
				else if (isEngine) { enginePositions.emplace_back(nx, ny); }

				if (hasOpenNeighbor(nx, ny)) { frontier.emplace_back(nx, ny); }
			}

			if (!hasOpenNeighbor(fromX, fromY)) {
				frontier[frontierIdx] = frontier.back();
				frontier.pop_back();
			}
		}

		if (static_cast<i32>(placed.size()) < targetParts) {
			EngineLogWarning("ShipCollisionTest::buildComplexShip: grid filled at {}/{} parts (frontier exhausted) -- consider a larger padding factor.",
				placed.size(), targetParts);
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
					EntityID ship = buildComplexShip(pos, facing, rng, 37, 2, 1, m_hardpointVariant, weaponVariant, m_engineVariant);

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

	void ShipCollisionTest::spawnTieredFleetBattle(d64 sideSpacing, d64 shipSpacing)
	{
		// Small fighter: single engine, fires fast and weak, short range (speed * 1.5s Lifetime),
		// stays quick by being both light and modestly thrust-heavy.
		PartVariantID smallWeapon = m_registry.registerVariant(PartVariant{
			.name = "Fighter Light Gun", .category = PartCategory::Weapon,
			.params = WeaponParams{ PartBaseStats{ 0.5f, 5.0f, 1.0f, 0.0f }, 3.0f, 0.3f, 10.0f, 0.003f, 1.5f }
			});
		PartVariantID smallEngine = m_registry.registerVariant(PartVariant{
			.name = "Fighter Engine", .category = PartCategory::Engine,
			.params = EngineParams{ PartBaseStats{ 2.0f, 10.0f, 2.0f, 0.0f }, 6.0f, 2.0f }
			});

		// Medium fighter: same archetype, a notch stronger/slower/tankier in every stat.
		PartVariantID mediumWeapon = m_registry.registerVariant(PartVariant{
			.name = "Fighter Medium Gun", .category = PartCategory::Weapon,
			.params = WeaponParams{ PartBaseStats{ 0.75f, 8.0f, 1.5f, 0.0f }, 8.0f, 0.6f, 6.0f, 0.004f }
			});
		PartVariantID mediumEngine = m_registry.registerVariant(PartVariant{
			.name = "Medium Fighter Engine", .category = PartCategory::Engine,
			.params = EngineParams{ PartBaseStats{ 3.0f, 15.0f, 3.0f, 0.0f }, 8.0f, 3.0f }
			});

		// Frigate: 4 size-1 hardpoints (m_hardpointVariant is already 1x1). Three gun profiles
		// (rapid/standard/siege) so the two sides' frigates don't necessarily bring matching
		// range/damage and simply trade blows at an identical stalemate range.
		PartVariantID frigateGunRapid = m_registry.registerVariant(PartVariant{
			.name = "Frigate Rapid Gun", .category = PartCategory::Weapon,
			.params = WeaponParams{ PartBaseStats{ 1.5f, 20.0f, 3.0f, 0.0f }, 22.0f, 1.0f, 10.0f, 0.008f }
			});
		PartVariantID frigateGunStandard = m_registry.registerVariant(PartVariant{
			.name = "Frigate Long Gun", .category = PartCategory::Weapon,
			.params = WeaponParams{ PartBaseStats{ 2.0f, 25.0f, 4.0f, 0.0f }, 40.0f, 2.5f, 16.0f, 0.01f }
			});
		PartVariantID frigateGunSiege = m_registry.registerVariant(PartVariant{
			.name = "Frigate Siege Gun", .category = PartCategory::Weapon,
			.params = WeaponParams{ PartBaseStats{ 2.5f, 30.0f, 5.0f, 0.0f }, 65.0f, 4.0f, 22.0f, 0.012f }
			});
		PartVariantID frigateEngine = m_registry.registerVariant(PartVariant{
			.name = "Frigate Engine", .category = PartCategory::Engine,
			.params = EngineParams{ PartBaseStats{ 6.0f, 30.0f, 6.0f, 0.0f }, 12.0f, 5.0f }
			});

		// Destroyer: ~90 parts (largest icon LOD tier), 3 engines, 8 size-1 hardpoints. Same
		// idea as the frigate -- flak/heavy/siege profiles instead of one fixed gun.
		PartVariantID destroyerGunFlak = m_registry.registerVariant(PartVariant{
			.name = "Destroyer Flak Gun", .category = PartCategory::Weapon,
			.params = WeaponParams{ PartBaseStats{ 2.5f, 35.0f, 5.0f, 0.0f }, 35.0f, 1.2f, 14.0f, 0.01f }
			});
		PartVariantID destroyerGunHeavy = m_registry.registerVariant(PartVariant{
			.name = "Destroyer Heavy Gun", .category = PartCategory::Weapon,
			.params = WeaponParams{ PartBaseStats{ 3.0f, 40.0f, 6.0f, 0.0f }, 70.0f, 3.5f, 20.0f, 0.015f }
			});
		PartVariantID destroyerGunSiege = m_registry.registerVariant(PartVariant{
			.name = "Destroyer Siege Gun", .category = PartCategory::Weapon,
			.params = WeaponParams{ PartBaseStats{ 4.0f, 50.0f, 7.0f, 0.0f }, 110.0f, 6.0f, 26.0f, 0.02f }
			});
		PartVariantID destroyerEngine = m_registry.registerVariant(PartVariant{
			.name = "Destroyer Engine", .category = PartCategory::Engine,
			.params = EngineParams{ PartBaseStats{ 8.0f, 40.0f, 8.0f, 0.0f }, 20.0f, 8.0f }
			});

		std::mt19937 rng{ std::random_device{}() };

		// Formation slots, laid out per side: 5 small fighters, 3 medium fighters, 1 frigate, 1 destroyer.
		// weaponOptions, when non-empty, overrides weapon/engageRange with a per-ship random pick
		// from {gun, matching engageRange} pairs -- lets frigates/destroyers vary ship-to-ship
		// instead of every ship in the tier bringing an identical gun.
		struct WeaponOption { PartVariantID weapon; f32 engageRange; };
		struct FleetSlot { i32 basePartCount, hardpointCount, engineCount; PartVariantID weapon, engine; f32 engageRange; std::vector<WeaponOption> weaponOptions{}; };
		const FleetSlot SMALL_FIGHTER{ 6, 1, 1, smallWeapon, smallEngine, 4.0f };    // ~15km gun range, brawls close
		const FleetSlot MEDIUM_FIGHTER{ 15, 2, 1, mediumWeapon, mediumEngine, 6.0f }; // ~18km gun range
		const FleetSlot FRIGATE{ 41, 4, 1, frigateGunStandard, frigateEngine, 40.0f,
			{ { frigateGunRapid, 25.0f }, { frigateGunStandard, 40.0f }, { frigateGunSiege, 55.0f } } };
		const FleetSlot DESTROYER{ 79, 8, 3, destroyerGunHeavy, destroyerEngine, 60.0f,
			{ { destroyerGunFlak, 40.0f }, { destroyerGunHeavy, 60.0f }, { destroyerGunSiege, 85.0f } } };

		std::vector<FleetSlot> formation;
		for (i32 i = 0; i < 5; i++) { formation.push_back(SMALL_FIGHTER); }
		for (i32 i = 0; i < 3; i++) { formation.push_back(MEDIUM_FIGHTER); }
		formation.push_back(FRIGATE);
		formation.push_back(DESTROYER);

		auto spawnSide = [&](d64 xOffset, f32 facing, i32 teamId) {
			d64 half = (static_cast<d64>(formation.size()) - 1) * shipSpacing / 2.0;
			for (size_t i = 0; i < formation.size(); i++) {
				const FleetSlot& slot = formation[i];
				PartVariantID weapon = slot.weapon;
				f32 engageRange = slot.engageRange;
				if (!slot.weaponOptions.empty()) {
					std::uniform_int_distribution<size_t> pickWeapon(0, slot.weaponOptions.size() - 1);
					const WeaponOption& option = slot.weaponOptions[pickWeapon(rng)];
					weapon = option.weapon;
					engageRange = option.engageRange;
				}

				Vector2double pos{ xOffset, static_cast<d64>(i) * shipSpacing - half };
				EntityID ship = buildComplexShip(pos, facing, rng, slot.basePartCount, slot.hardpointCount, slot.engineCount,
					m_hardpointVariant, weapon, slot.engine);

				m_ecs.addComponent(ship, Faction{ .teamId = teamId });
				m_ecs.addComponent(ship, Separation{ .margin = 0.1f });
				m_ecs.addComponent(ship, AIController{
					.target = EntityID{},
					.engageRange = engageRange,
					});
			}
			};

		// Two fleets facing each other, separated by sideSpacing along X -- team 0 faces +x
		// (rotation 0), team 1 faces -x (rotation pi).
		spawnSide(-sideSpacing / 2.0, 0.0f, 0);
		spawnSide(sideSpacing / 2.0, 3.14159265f, 1);

		EngineLogInfo("ShipCollisionTest: spawned tiered fleet battle ({} small fighters, {} medium fighters, {} frigate, {} destroyer per side, {} total).",
			5, 3, 1, 1, formation.size() * 2);
	}

	void ShipCollisionTest::spawnMassiveShipTest(Vector2double pos)
	{
		PartVariantID weapon = m_registry.registerVariant(PartVariant{
			.name = "Massive Test Gun", .category = PartCategory::Weapon,
			.params = WeaponParams{ PartBaseStats{ 2.0f, 25.0f, 4.0f, 0.0f }, 40.0f, 2.5f, 16.0f, 0.01f }
			});
		PartVariantID engine = m_registry.registerVariant(PartVariant{
			.name = "Massive Test Engine", .category = PartCategory::Engine,
			.params = EngineParams{ PartBaseStats{ 6.0f, 30.0f, 6.0f, 0.0f }, 12.0f, 5.0f }
			});

		std::mt19937 rng{ std::random_device{}() };
		i32 hardpointCount = 12;
		i32 engineCount = 6;
		i32 basePartCount = 250000 - hardpointCount - engineCount;

		EntityID ship = buildComplexShip(pos, 0.0f, rng, basePartCount, hardpointCount, engineCount,
			m_hardpointVariant, weapon, engine);

		m_ecs.addComponent(ship, Faction{ .teamId = 0 });
		m_ecs.addComponent(ship, Separation{ .margin = 0.1f });

		EngineLogInfo("ShipCollisionTest: spawned massive ship stress test ({} parts, entity {}).", 250000, ship.id);
	}
}