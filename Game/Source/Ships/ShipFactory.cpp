#include <Ships/ShipFactory.h>
#include <Ships/BakedShipStats.h>
#include <Components/Position.h>
#include <Components/Movement.h>
#include <Components/Physics.h>
#include <Components/Stability.h>
#include <Components/Health.h>
#include <Components/Thruster.h>
#include <Components/MovementDamper.h>
#include <Components/Mount.h>
#include <Components/Weapon.h>
#include <Components/Renderable.h>
#include <Components/ShipVisual.h>
#include <Graphics/MeshID.h>
#include <tuple>
#include <algorithm>

namespace Engine {
	constexpr f32 THRUST_FORCE_MULTIPLIER = 2.0f;
	constexpr f32 TURRET_ART_ROTATION_OFFSET = -1.57079632679f;
	constexpr f32 TURRET_ART_SCALE_MULTIPLIER = 2.0f;
	constexpr f32 SHIP_FACTORY_PI = 3.14159265f;

	namespace {
		f32 normalizeAngle(f32 a) {
			while (a > SHIP_FACTORY_PI) { a -= 2.0f * SHIP_FACTORY_PI; }
			while (a < -SHIP_FACTORY_PI) { a += 2.0f * SHIP_FACTORY_PI; }
			return a;
		}

		// Event-sweep over circular interval start/end points to find the angle
		// covered by the most overlapping weapon traverse arcs, wraparound-safe.
		f32 computeIdealFiringHeading(const std::vector<std::pair<f32, f32>>& arcs) {
			if (arcs.empty()) { return 0.0f; }

			struct Event { f32 angle; i32 delta; };
			std::vector<Event> events;
			events.reserve(arcs.size() * 2);

			for (auto& [minAbs, maxAbs] : arcs) {
				f32 start = normalizeAngle(minAbs);
				f32 end = normalizeAngle(maxAbs);
				if (start <= end) {
					events.push_back({ start, +1 });
					events.push_back({ end, -1 });
				}
				else {
					events.push_back({ start, +1 });
					events.push_back({ SHIP_FACTORY_PI, -1 });
					events.push_back({ -SHIP_FACTORY_PI, +1 });
					events.push_back({ end, -1 });
				}
			}

			std::sort(events.begin(), events.end(), [](const Event& a, const Event& b) { return a.angle < b.angle; });

			i32 count = 0;
			i32 bestCount = 0;
			f32 bestStart = -SHIP_FACTORY_PI, bestEnd = SHIP_FACTORY_PI;
			i32 n = static_cast<i32>(events.size());
			for (i32 i = 0; i < n; i++) {
				f32 segStart = events[i].angle;
				count += events[i].delta;
				f32 segEnd = (i + 1 < n) ? events[i + 1].angle : SHIP_FACTORY_PI;
				if (segEnd <= segStart) { continue; }

				if (count > bestCount || (count == bestCount && count > 0 && (segEnd - segStart) > (bestEnd - bestStart))) {
					bestCount = count;
					bestStart = segStart;
					bestEnd = segEnd;
				}
			}

			if (bestCount == 0) { return 0.0f; }
			return (bestStart + bestEnd) * 0.5f;
		}
	}

	ShipFactory::ShipFactory(const ShipFactoryDesc& desc) : Base(desc.base),
		m_ecs(desc.ecs),
		m_partReg(desc.partReg)
	{
	}

	ShipFactory::~ShipFactory()
	{
	}

	EntityID ShipFactory::bake(const ShipGrid& grid, Vector2double spawnPos, f32 spawnRotation,
		const std::unordered_map<std::pair<i32, i32>, PartVariantID, PairHash>& hardpointLoadout)
	{
		f32 totalMass = 0.0f;
		f32 totalStability = 0.0f;
		f32 totalThrustForce = 0.0f;

		ShipVisual visual;
		std::vector<std::tuple<i32, i32, const HardpointParams*, i32>> hardpoints;

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
						i32 visualPartIndex = static_cast<i32>(visual.parts.size());
						visual.parts.push_back({
							localOffsetFor(x, y, cell.sizeX, cell.sizeY), cell.sizeX, cell.sizeY, PartCategory::Hardpoint, cell.variant
							});
						hardpoints.emplace_back(x, y, &params, visualPartIndex);
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

		if (totalThrustForce > 0.0f) {
			m_ecs.addComponent(ship, Thruster{ .maxAccel = (totalThrustForce * THRUST_FORCE_MULTIPLIER) / totalMass, .throttle = 0.0f });
			m_ecs.addComponent(ship, MovementDamper{});
		}

		std::vector<std::pair<f32, f32>> weaponArcs;

		for (auto& [hx, hy, params, visualPartIndex] : hardpoints) {
			Vector2float offset{
				(static_cast<f32>(hx) + params->sizeX * 0.5f - grid.width() * 0.5f) * static_cast<f32>(GRID_CELL_SIZE_KM),
				(static_cast<f32>(hy) + params->sizeY * 0.5f - grid.height() * 0.5f) * static_cast<f32>(GRID_CELL_SIZE_KM)
			};

			// No per-hardpoint rotation authoring data exists yet (grid/loadout carry no such field); mounts default to facing 0.
			f32 mountRotation = 0.0f;

			auto loadoutIt = hardpointLoadout.find({ hx, hy });

			Vector2float mountOffset = offset;
			if (loadoutIt != hardpointLoadout.end()) {
				const WeaponParams& weaponParams = std::get<WeaponParams>(m_partReg.get(loadoutIt->second).params);
				mountOffset = offset + weaponParams.anchorOffset;
				visual.parts[visualPartIndex].anchorOffset = weaponParams.anchorOffset;
			}

			EntityID mountEntity = m_ecs.createEntity();
			m_ecs.addComponent(mountEntity, Mount{ .owner = ship, .offset = mountOffset, .rotation = mountRotation });
			m_ecs.addComponent(mountEntity, Position{ .transform = spawnPos, .rotation = spawnRotation }); // corrected next tick by MountFollowSystem
			m_ecs.addComponent(mountEntity, Renderable{
				.mesh = MeshID::Quad, .texture = TextureID::SmallCannon,
				.scale = std::max(params->sizeX, params->sizeY) * static_cast<f32>(GRID_CELL_SIZE_KM) * TURRET_ART_SCALE_MULTIPLIER,
				.rotationOffset = TURRET_ART_ROTATION_OFFSET
				});
			m_ecs.addComponent(mountEntity, Health{ .current = params->health, .max = params->health });

			if (loadoutIt != hardpointLoadout.end()) {
				Weapon weapon = m_partReg.buildWeaponFromVariant(loadoutIt->second, mountRotation);
				weaponArcs.emplace_back(weapon.minRotationAbs, weapon.maxRotationAbs);
				m_ecs.addComponent(mountEntity, weapon);
			}
			// No matching entity, mount stays empty


			visual.parts[visualPartIndex].linkedEntity = mountEntity;
		}

		m_ecs.addComponent(ship, grid.toRuntimeData());
		m_ecs.addComponent(ship, BakedShipStats{ .idealFiringHeading = computeIdealFiringHeading(weaponArcs) });

		m_ecs.addComponent(ship, visual);

		EngineLogInfo("ShipFactory: baked ship, mass {}, stability {}, {} visual part(s), {} hardpoint(s)",
			totalMass, totalStability, visual.parts.size(), hardpoints.size());

		return ship;
	}
}