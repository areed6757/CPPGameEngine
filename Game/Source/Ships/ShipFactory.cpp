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
		f32 totalStability = 0.0f;
		f32 totalThrustForce = 0.0f;

		ShipVisual visual;
		std::vector<std::tuple<i32, i32, const HardpointParams*>> hardpoints;

		auto localOffsetFor = [&](i32 x, i32 y, i32 sizeX, i32 sizeY) -> Vector2float {
			return Vector2float{
				(static_cast<f32>(x) + sizeX * 0.5f - grid.width() * 0.5f) * static_cast<f32>(GRID_CELL_SIZE_KM),
				(static_cast<f32>(y) + sizeY * 0.5f - grid.height() * 0.5f) * static_cast<f32>(GRID_CELL_SIZE_KM)
			};
			};

		for (i32 y = 0; y < grid.height(); y++) {
			for (i32 x = 0; x < grid.width(); x++) {
				const GridCell& cell = grid.at(x, y);
				if (cell.partCategory == PartCategory::None || !cell.isAnchor) { continue; }

				const PartVariant& variant = m_partReg.get(cell.variant);
				std::visit([&](auto&& params) {
					using T = std::decay_t<decltype(params)>;
					if constexpr (std::is_same_v<T, HullParams>) {
						totalMass += params.mass;
						totalStability += params.stabilityContribution;
						visual.parts.push_back({
							localOffsetFor(x, y, cell.sizeX, cell.sizeY), cell.sizeX, cell.sizeY, PartCategory::Hull, cell.variant
							});
					}
					else if constexpr (std::is_same_v<T, ArmorParams>) {
						totalMass += params.mass;
						totalStability += params.stabilityContribution;
						visual.parts.push_back({
							localOffsetFor(x, y, cell.sizeX, cell.sizeY), cell.sizeX, cell.sizeY, PartCategory::Armor, cell.variant
							});
					}
					else if constexpr (std::is_same_v<T, EngineParams>) {
						totalMass += params.mass;
						totalStability += params.stabilityContribution;
						totalThrustForce += params.thrustForce;
						visual.parts.push_back({
							localOffsetFor(x, y, cell.sizeX, cell.sizeY), cell.sizeX, cell.sizeY, PartCategory::Engine, cell.variant
							});
					}
					else if constexpr (std::is_same_v<T, HardpointParams>) {
						totalMass += params.mass;
						totalStability += params.stabilityContribution;
						visual.parts.push_back({
							localOffsetFor(x, y, cell.sizeX, cell.sizeY), cell.sizeX, cell.sizeY, PartCategory::Hardpoint, cell.variant
							});
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
		m_ecs.addComponent(ship, Stability{ .current = totalStability, .max = totalStability });
		m_ecs.addComponent(ship, grid.buildCollisionGeometry());
		m_ecs.addComponent(ship, grid.toRuntimeData());
		m_ecs.addComponent(ship, visual);

		if (totalThrustForce > 0.0f) {
			m_ecs.addComponent(ship, Thruster{ .maxAccel = totalThrustForce / totalMass, .throttle = 0.0f });
		}

		for (auto& [hx, hy, params] : hardpoints) {
			Vector2float offset{
				(static_cast<f32>(hx) + params->sizeX * 0.5f - grid.width() * 0.5f) * static_cast<f32>(GRID_CELL_SIZE_KM),
				(static_cast<f32>(hy) + params->sizeY * 0.5f - grid.height() * 0.5f) * static_cast<f32>(GRID_CELL_SIZE_KM)
			};

			// Every hardpoint unconditionally spawns a weapon until empty-socket loadout support is added
			// TODO: Refactor this entire block
			EntityID weaponEntity = m_ecs.createEntity();
			m_ecs.addComponent(weaponEntity, WeaponMount{ .owner = ship, .offset = offset });
			m_ecs.addComponent(weaponEntity, Position{ .transform = spawnPos, .rotation = spawnRotation }); // corrects via MountFollowSystem
			m_ecs.addComponent(weaponEntity, Renderable{
				.mesh = MeshID::Quad, .texture = std::nullopt,
				.scale = std::max(params->sizeX, params->sizeY) * static_cast<f32>(GRID_CELL_SIZE_KM) * 0.5f
				});
			m_ecs.addComponent(weaponEntity, Health{ .current = params->health, .max = params->health });
			m_ecs.addComponent(weaponEntity, Weapon{
				.cooldown = 1.0f, .timeSinceLastFire = 0.0f,
				.projectileSpeed = 35.0f, .projectileRadius = 0.01f, .projectileDamage = 5.0f
				});
		}

		// EngineLogInfo("ShipFactory: baked ship, mass {}, stability {}, {} visual part(s), {} hardpoint(s)", totalMass, totalStability, visual.parts.size(), hardpoints.size());

		return ship;
	}
}