// ShipCollisionTest.cpp
#include <Test/ShipCollisionTest.h>
#include <Ships/ShipGrid.h>
#include <Components/Position.h>
#include <Components/Movement.h>
#include <Components/Physics.h>
#include <Components/Renderable.h>
#include <Components/DamagePayload.h>
#include <Components/Lifetime.h>
#include <Graphics/MeshID.h>

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
	}

	ShipCollisionTest::~ShipCollisionTest() {}

	EntityID ShipCollisionTest::buildSmallShip(Vector2double pos, f32 rotation)
	{
		ShipGridDesc sgDesc{ {m_logger}, 5, 5 };
		ShipGrid grid(sgDesc);
		grid.tryPlacePart(0, 0, 5, 5, PartCategory::Hull, m_hullVariant);
		return m_shipFactory.bake(grid, pos, rotation);
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
}