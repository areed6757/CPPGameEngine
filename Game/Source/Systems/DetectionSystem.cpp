#include <Systems/DetectionSystem.h>
#include <algorithm>
#include <cmath>

namespace Engine {
	// tuned against ShipCollisionTest's fleet-battle content: fighters resolve ~7-16km,
	// frigates ~40km, destroyers ~90km, bracketing their respective engageRange bands
	constexpr d64 SIGNAL_RANGE_PER_MAGNITUDE_KM = 10.0;
	constexpr d64 SIGNAL_DETECTION_SEARCH_RADIUS_KM = 300.0;
	constexpr f32 SIGNAL_DETECTION_MIN_RESOLUTION = 0.02f;

	DetectionSystem::DetectionSystem(const DetectionSystemDesc& desc) : Base(desc.base),
		m_ecs(desc.ecs),
		m_signalTree(desc.signalTree),
		m_threadPool(desc.threadPool)
	{
		m_entityMask = m_ecs.makeSignature<AIController, Position, Faction, Sensor>();
		m_reads = m_ecs.makeSignature<AIController, Position, Faction, Sensor, SignalSignature>();
		m_writes = m_ecs.makeSignature<AIController>();
	}

	DetectionSystem::~DetectionSystem()
	{
	}

	void DetectionSystem::Update(d64 dt)
	{
		i32 c = m_ecs.sizeComponentPool<AIController>();
		i32 threadCount = m_threadPool.threadCount();
		i32 chunkSize = (c + threadCount - 1) / threadCount;

		for (i32 t = 0; t < threadCount; t++) {
			i32 start = t * chunkSize;
			i32 end = std::min(start + chunkSize, c);
			if (start >= end) { continue; }

			m_threadPool.submit([this, start, end]() {
				std::vector<EntityID> nearby; // thread-local, reused across this chunk's entities

				for (i32 i = start; i < end; i++) {
					i32 entityIndex = m_ecs.entityAtDenseIndex<AIController>(i);
					EntityID id = m_ecs.entityFromIndex(entityIndex);
					if ((m_ecs.getSignature(id) & m_entityMask) != m_entityMask) { continue; }

					auto& pos = m_ecs.getComponent<Position>(id);
					i32 myTeam = m_ecs.getComponent<Faction>(id).teamId;
					f32 sensorPower = m_ecs.getComponent<Sensor>(id).power;

					AABB searchBounds{
						Vector2double{ pos.transform.x - SIGNAL_DETECTION_SEARCH_RADIUS_KM, pos.transform.y - SIGNAL_DETECTION_SEARCH_RADIUS_KM },
						Vector2double{ pos.transform.x + SIGNAL_DETECTION_SEARCH_RADIUS_KM, pos.transform.y + SIGNAL_DETECTION_SEARCH_RADIUS_KM }
					};

					nearby.clear();
					m_signalTree.query(searchBounds, nearby);

					EntityID best{};
					f32 bestResolution = 0.0f;

					for (EntityID other : nearby) {
						if (other.id == id.id) { continue; }
						if (!m_ecs.hasComponent<Faction>(other) || !m_ecs.hasComponent<SignalSignature>(other)) { continue; }
						if (m_ecs.getComponent<Faction>(other).teamId == myTeam) { continue; }

						auto& otherPos = m_ecs.getComponent<Position>(other);
						auto& otherSignal = m_ecs.getComponent<SignalSignature>(other);

						Vector2double delta = otherPos.transform - pos.transform;
						d64 dist = std::sqrt(delta.x * delta.x + delta.y * delta.y);

						d64 effectiveRange = static_cast<d64>(otherSignal.magnitude) * static_cast<d64>(sensorPower) * SIGNAL_RANGE_PER_MAGNITUDE_KM;
						if (effectiveRange <= 0.0) { continue; }

						f32 resolution = static_cast<f32>(std::clamp(1.0 - dist / effectiveRange, 0.0, 1.0));
						if (resolution > bestResolution) {
							bestResolution = resolution;
							best = other;
						}
					}

					auto& ai = m_ecs.getComponentAtDenseIndex<AIController>(i);
					EntityID resolved = (bestResolution >= SIGNAL_DETECTION_MIN_RESOLUTION) ? best : EntityID{};

					// a changed lock (new target, or lock lost/regained) invalidates the chase-pulse distance sample
					if (resolved.id != ai.target.id || resolved.generation != ai.target.generation) {
						ai.lastTargetDist = -1.0;
						ai.throttleHeld = false;
					}

					ai.target = resolved;
					ai.targetResolution = m_ecs.isValidEntity(resolved) ? bestResolution : 0.0f;
				}
				});
		}

		m_threadPool.waitForAll();
	}
}
