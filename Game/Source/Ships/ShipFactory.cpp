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
#include <Components/UtilityAIState.h>
#include <Components/SignalSignature.h>
#include <Components/Sensor.h>
#include <Graphics/MeshID.h>
#include <tuple>
#include <algorithm>

namespace Engine {
	constexpr f32 THRUST_FORCE_MULTIPLIER = 2.0f;
	constexpr f32 TURRET_ART_ROTATION_OFFSET = -HALF_PI;
	constexpr f32 TURRET_ART_SCALE_MULTIPLIER = 2.0f;

	constexpr i32 SHIP_CLASS_FRIGATE_MIN_PARTS = 20;
	constexpr i32 SHIP_CLASS_DESTROYER_MIN_PARTS = 60;
	constexpr i32 SHIP_CLASS_CRUISER_MIN_PARTS = 150;
	constexpr i32 SHIP_CLASS_BATTLESHIP_MIN_PARTS = 500;

	constexpr f32 POINT_DEFENSE_DPS_THRESHOLD = 0.5f;

	namespace {
		f32 normalizeAngle(f32 a) {
			while (a > PI) { a -= 2.0f * PI; }
			while (a < -PI) { a += 2.0f * PI; }
			return a;
		}

		ShipClass classifyShipSize(i32 totalPartCount) {
			if (totalPartCount < SHIP_CLASS_FRIGATE_MIN_PARTS) { return ShipClass::Fighter; }
			if (totalPartCount < SHIP_CLASS_DESTROYER_MIN_PARTS) { return ShipClass::Frigate; }
			if (totalPartCount < SHIP_CLASS_CRUISER_MIN_PARTS) { return ShipClass::Destroyer; }
			if (totalPartCount < SHIP_CLASS_BATTLESHIP_MIN_PARTS) { return ShipClass::Cruiser; }
			return ShipClass::Battleship;
		}

		const char* weaponRoleName(WeaponRole role) {
			switch (role) {
			case WeaponRole::PointDefense:   return "PointDefense";
			case WeaponRole::Flak:           return "Flak";
			case WeaponRole::LightPrimary:   return "LightPrimary";
			case WeaponRole::StandardPrimary:return "StandardPrimary";
			case WeaponRole::RapidPrimary:   return "RapidPrimary";
			case WeaponRole::HeavyPrimary:   return "HeavyPrimary";
			case WeaponRole::SiegePrimary:   return "SiegePrimary";
			case WeaponRole::Torpedo:        return "Torpedo";
			default:                         return "Unknown";
			}
		}

		const char* shipClassName(ShipClass shipClass) {
			switch (shipClass) {
			case ShipClass::Fighter:    return "Fighter";
			case ShipClass::Frigate:    return "Frigate";
			case ShipClass::Destroyer:  return "Destroyer";
			case ShipClass::Cruiser:    return "Cruiser";
			case ShipClass::Battleship: return "Battleship";
			default:                    return "Unknown";
			}
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
					events.push_back({ PI, -1 });
					events.push_back({ -PI, +1 });
					events.push_back({ end, -1 });
				}
			}

			std::sort(events.begin(), events.end(), [](const Event& a, const Event& b) { return a.angle < b.angle; });

			i32 count = 0;
			i32 bestCount = 0;
			f32 bestStart = -PI, bestEnd = PI;
			i32 n = static_cast<i32>(events.size());
			for (i32 i = 0; i < n; i++) {
				f32 segStart = events[i].angle;
				count += events[i].delta;
				f32 segEnd = (i + 1 < n) ? events[i + 1].angle : PI;
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
		f32 systemCapacityMax = 0.0f;
		f32 systemCapacityUsed = 0.0f;
		f32 totalSignalEmission = 0.0f;
		f32 totalSensorPower = 0.0f;

		auto applySystemCapacity = [&](f32 contribution) {
			if (contribution > 0.0f) { systemCapacityMax += contribution; }
			else { systemCapacityUsed += -contribution; }
			};

		ShipVisual visual;
		std::vector<std::tuple<i32, i32, const HardpointParams*, i32, f32>> hardpoints;

		auto localOffsetFor = [&](i32 x, i32 y, i32 sizeX, i32 sizeY) -> Vector2float {
			return gridAxesToLocal(
				(static_cast<f32>(x) + sizeX * 0.5f - grid.width() * 0.5f) * static_cast<f32>(GRID_CELL_SIZE_KM),
				(static_cast<f32>(y) + sizeY * 0.5f - grid.height() * 0.5f) * static_cast<f32>(GRID_CELL_SIZE_KM)
			);
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
						applySystemCapacity(params.systemCapacityContribution);
						totalSignalEmission += params.signalEmissionValue;
						totalSensorPower += params.sensorPowerValue;
						visual.parts.push_back({
							localOffsetFor(x, y, cell.sizeX, cell.sizeY), cell.sizeX, cell.sizeY, PartCategory::Hull, cell.variant
							});
					}
					else if constexpr (std::is_same_v<T, ArmorParams>) {
						totalMass += params.mass;
						totalStability += params.stabilityContribution;
						applySystemCapacity(params.systemCapacityContribution);
						totalSignalEmission += params.signalEmissionValue;
						totalSensorPower += params.sensorPowerValue;
						visual.parts.push_back({
							localOffsetFor(x, y, cell.sizeX, cell.sizeY), cell.sizeX, cell.sizeY, PartCategory::Armor, cell.variant
							});
					}
					else if constexpr (std::is_same_v<T, EngineParams>) {
						totalMass += params.mass;
						totalStability += params.stabilityContribution;
						totalThrustForce += params.thrustForce;
						applySystemCapacity(params.systemCapacityContribution);
						totalSignalEmission += params.signalEmissionValue;
						totalSensorPower += params.sensorPowerValue;
						visual.parts.push_back({
							localOffsetFor(x, y, cell.sizeX, cell.sizeY), cell.sizeX, cell.sizeY, PartCategory::Engine, cell.variant
							});
					}
					else if constexpr (std::is_same_v<T, HardpointParams>) {
						totalMass += params.mass;
						totalStability += params.stabilityContribution;
						applySystemCapacity(params.systemCapacityContribution);
						totalSignalEmission += params.signalEmissionValue;
						totalSensorPower += params.sensorPowerValue;
						i32 visualPartIndex = static_cast<i32>(visual.parts.size());
						visual.parts.push_back({
							localOffsetFor(x, y, cell.sizeX, cell.sizeY), cell.sizeX, cell.sizeY, PartCategory::Hardpoint, cell.variant
							});
						hardpoints.emplace_back(x, y, &params, visualPartIndex, cell.rotation);
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

		f32 totalDps = 0.0f;
		f32 pointDefenseDps = 0.0f;
		f32 dpsWeightedRangeSum = 0.0f;
		f32 bestVolleyDamage = -1.0f;
		f32 primaryRange = 0.0f;

		for (auto& [hx, hy, params, visualPartIndex, hardpointRotation] : hardpoints) {
			Vector2float offset = gridAxesToLocal(
				(static_cast<f32>(hx) + params->sizeX * 0.5f - grid.width() * 0.5f) * static_cast<f32>(GRID_CELL_SIZE_KM),
				(static_cast<f32>(hy) + params->sizeY * 0.5f - grid.height() * 0.5f) * static_cast<f32>(GRID_CELL_SIZE_KM)
			);

			f32 mountRotation = hardpointRotation;

			auto loadoutIt = hardpointLoadout.find({ hx, hy });

			Vector2float mountOffset = offset;
			if (loadoutIt != hardpointLoadout.end()) {
				const WeaponParams& weaponParams = std::get<WeaponParams>(m_partReg.get(loadoutIt->second).params);
				mountOffset = offset + weaponParams.anchorOffset;
				visual.parts[visualPartIndex].anchorOffset = weaponParams.anchorOffset;
				applySystemCapacity(weaponParams.systemCapacityContribution);
				totalSignalEmission += weaponParams.signalEmissionValue;
				totalSensorPower += weaponParams.sensorPowerValue;
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

				f32 volleyDamage = weapon.projectileDamage * static_cast<f32>(weapon.barrelCount);
				f32 dps = weapon.cooldown > 0.0f ? volleyDamage / weapon.cooldown : volleyDamage;

				totalDps += dps;
				dpsWeightedRangeSum += dps * weapon.idealRange;
				if (weapon.role == WeaponRole::PointDefense) { pointDefenseDps += dps; }
				if (volleyDamage > bestVolleyDamage) {
					bestVolleyDamage = volleyDamage;
					primaryRange = weapon.maxRange;
				}

				EngineLogInfo("ShipFactory:   hardpoint ({}, {}) weapon: role={}, maxRange={:.2f}, idealRange={:.2f}, volleyDamage={:.1f}, dps={:.1f}",
					hx, hy, weaponRoleName(weapon.role), weapon.maxRange, weapon.idealRange, volleyDamage, dps);

				m_ecs.addComponent(mountEntity, weapon);
			}
			// No matching entity, mount stays empty


			visual.parts[visualPartIndex].linkedEntity = mountEntity;
		}

		m_ecs.addComponent(ship, grid.toRuntimeData());

		BakedShipStats stats{
			.idealFiringHeading = computeIdealFiringHeading(weaponArcs),
			.primaryRange = primaryRange,
			.idealRange = totalDps > 0.0f ? dpsWeightedRangeSum / totalDps : 0.0f,
			.isPointDefense = totalDps > 0.0f && (pointDefenseDps / totalDps) > POINT_DEFENSE_DPS_THRESHOLD,
			.systemCapacityMax = systemCapacityMax,
			.systemCapacityUsed = systemCapacityUsed,
			.isOverSystemCapacity = systemCapacityUsed > systemCapacityMax,
			.totalSignalEmission = totalSignalEmission,
			.totalSensorPower = totalSensorPower,
			.shipClass = classifyShipSize(static_cast<i32>(visual.parts.size())),
		};
		EngineLogInfo("ShipFactory: baked stats -- class={}, idealFiringHeading={:.2f}, primaryRange={:.2f}, idealRange={:.2f}, isPointDefense={}, isUtility={}, systemCapacity={:.1f}/{:.1f}{}, signalEmission={:.1f}, sensorPower={:.2f}",
			shipClassName(stats.shipClass), stats.idealFiringHeading, stats.primaryRange, stats.idealRange, stats.isPointDefense, stats.isUtility,
			stats.systemCapacityUsed, stats.systemCapacityMax, stats.isOverSystemCapacity ? " (OVER CAPACITY)" : "", stats.totalSignalEmission, 1.0f + stats.totalSensorPower);

		m_ecs.addComponent(ship, stats);
		m_ecs.addComponent(ship, UtilityAIState{});
		m_ecs.addComponent(ship, SignalSignature{ .magnitude = totalSignalEmission });
		m_ecs.addComponent(ship, Sensor{ .power = 1.0f + totalSensorPower });

		m_ecs.addComponent(ship, visual);

		EngineLogInfo("ShipFactory: baked ship, mass {}, stability {}, {} visual part(s), {} hardpoint(s)",
			totalMass, totalStability, visual.parts.size(), hardpoints.size());

		return ship;
	}
}