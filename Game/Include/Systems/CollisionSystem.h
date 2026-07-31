#pragma once
#include <Game.h>
#include <vector>

namespace Engine {
	struct CollisionSystemDesc {
		BaseDesc base;
		GameECSWrapper& ecs;
		AABBTree& aabbTree;
	};

	struct CollisionCandidate {
		EntityID entityA;
		EntityID entityB;
	};

	struct CollisionEvent {
		EntityID entityA;
		EntityID entityB;

		Vector2float impulseA{};
		Vector2float impulseB{};

		Vector2double hitPoint{};
	};

	class CollisionSystem : public Base, public TickedSystem {
	public:
		explicit CollisionSystem(const CollisionSystemDesc& desc);
		~CollisionSystem();

		void Update(d64 dt) override;

		[[nodiscard]] const std::vector<CollisionEvent>& getEvents() const noexcept;

	private:
		GameECSWrapper& m_ecs;
		GameCommandBuffer m_cmdBuffer;
		AABBTree& m_aabbTree;

		struct ProxyEntry { i32 proxyId = -1; i32 generation = -1; };
		std::vector<ProxyEntry> m_proxies;

		std::bitset<64> m_entityMask;
		std::bitset<64> m_movementMask;
		std::bitset<64> m_nonPhysicsCollisionMask;

		std::vector<CollisionCandidate> m_candidates{};
		std::vector<EntityID> m_nearbyScratch{};
		std::vector<CollisionEvent> m_events{};

		void broadPhase(std::vector<CollisionCandidate>& outCandidates);
		
		[[nodiscard]] bool narrowPhaseSwept(EntityID a, EntityID b, d64 dt, Vector2double& outHitPoint) const;
		[[nodiscard]] bool narrowPhaseShip(EntityID a, EntityID b, d64 dt, Vector2double& outHitPoint) const;
		[[nodiscard]] void computeImpulse(EntityID a, EntityID b, Vector2float& outImpulseA, Vector2float& outImpulseB) const;
		[[nodiscard]] bool narrowPhaseProjectileVsShip(EntityID projectile, EntityID ship, d64 dt, Vector2double& outHitPoint) const;
	};
}