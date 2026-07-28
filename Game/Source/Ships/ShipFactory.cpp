#include <Ships/ShipFactory.h>
#include <tuple>

namespace Engine {
	ShipFactory::ShipFactory(const ShipFactoryDesc& desc) : Base(desc.base),
		m_ecs(desc.ecs),
		m_partReg(desc.partReg)
	{

	}

	ShipFactory::~ShipFactory()
	{

	}

	EntityID ShipFactory::bake(const ShipGrid& grid, Vector2double spawnPos, f32 spawnRotation)
	{
		f32 totalMass = 0.0f;
		f32 totalHealth = 0.0f;
		f32 totalThrustForce = 0.0f;

		std::vector<std::tuple<i32, i32, const HardpointParams*>> hardpoints;

		for (i32 y = 0; y < grid.height(); y++) {
			for (i32 x = 0; x < grid.width(); x++) {
				const GridCell& cell = grid.at(x, y);
				if (cell.partCategory == PartCategory::None || !cell.isAnchor) { continue; }

				const PartVariant& variant = m_partReg.get(cell.variant);
				std::visit([&](auto&& params) {
					using T = std::decay_t<decltype(params)>;
					if constexpr (std::is_same_v<T, HullParams>) {
						totalMass += params.mass;
						totalHealth += params.health;
					}
					else if constexpr (std::is_same_v<T, ArmorParams>) {
						totalMass += params.mass;
					}
					else if constexpr (std::is_same_v<T, EngineParams>) {
						totalMass += params.mass;
						totalThrustForce += params.thrustForce;
					}
					else if constexpr (std::is_same_v<T, HardpointParams>) {
						totalMass += params.mass;
						hardpoints.emplace_back(x, y, &params);
					}
				}, variant.params);
			}
		}

		f32 halfDiag = 0.5f * std::sqrt(static_cast<f32>(grid.width() * grid.width() + grid.height() * grid.height()));
		f32 radius = halfDiag * static_cast<f32>(GRID_CELL_SIZE_KM);

		EntityID ship = m_ecs.createEntity();

		m_ecs.addComponent(ship, Position{ .transform = spawnPos, .rotation = spawnRotation });
		m_ecs.addComponent(ship, Movement{});
		m_ecs.addComponent(ship, Physics{ .radius = radius, .mass = totalMass > 0.0f ? totalMass : 1.0f });
		m_ecs.addComponent(ship, Health{ .current = totalHealth, .max = totalHealth });
		m_ecs.addComponent(ship, Renderable{ .mesh = MeshID::Quad, .texture = std::nullopt, .scale = radius * 2.0f });

		if (totalThrustForce > 0.0f) { m_ecs.addComponent(ship, Thruster{ .maxAccel = totalThrustForce / totalMass, .throttle = 0.0f }); };


		// Add weapons to hardpoints
		for (auto& [hx, hy, params] : hardpoints) {
			Vector2float localOffset{
				(static_cast<f32>(hx) + params->sizeX * 0.5f - grid.width() * 0.5f) * static_cast<f32>(GRID_CELL_SIZE_KM),
				(static_cast<f32>(hy) + params->sizeY * 0.5f - grid.height() * 0.5f) * static_cast<f32>(GRID_CELL_SIZE_KM)
			};

			EntityID weaponEntity = m_ecs.createEntity();
			m_ecs.addComponent(weaponEntity, WeaponMount{ .owner = ship, .offset = localOffset });
			// TODO: Actual weapon variants need to have a separate runtime structure, not just this grid
			m_ecs.addComponent(weaponEntity, Weapon{ .cooldown = 1.0f, .timeSinceLastFire = 0.0f,
				.projectileSpeed = 10.0f, .projectileRadius = 0.1f, .projectileDamage = 5.0f });
		}

		EngineLogInfo("ShipFactory: baked ship, mass {}, health {}, {} hardpoint(s)",
			totalMass, totalHealth, hardpoints.size() );

		return ship;
	}
}