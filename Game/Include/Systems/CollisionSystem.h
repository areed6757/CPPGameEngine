#pragma once
#include <Core/Common.h>
#include <Utilities/QuadTree.h>
#include <ECS/TickedSystem.h>
#include <GameECS.h>
#include <vector>
#include <Physics/Vector2float.h>

namespace Engine {
	struct CollisionSystemDesc {
		BaseDesc base;
		GameECSWrapper& ecs;
		QuadTree& quadtree;
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
		// TODO: Add damage payload fields here?
	};

	class CollisionSystem : public Base, public TickedSystem {
	public:
		explicit CollisionSystem(const CollisionSystemDesc& desc);
		~CollisionSystem();

		void Update(d64 dt) override;

		[[nodiscard]] const std::vector<CollisionEvent>& getEvents() const noexcept;

		std::bitset<64> getReadSignature() const noexcept { return m_reads; }
		std::bitset<64> getWriteSignature() const noexcept { return m_reads; }

	protected:
		std::bitset<64> m_reads{};
		std::bitset<64> m_writes{};

	private:
		GameECSWrapper& m_ecs;
		QuadTree& m_quadtree;
		std::bitset<64> m_entityMask;
		std::bitset<64> m_movementMask;
		std::bitset<64> m_nonPhysicsCollisionMask;

		std::vector<CollisionCandidate> m_candidates{};
		std::vector<EntityID> m_nearbyScratch{};
		std::vector<CollisionEvent> m_events{};

		d64 m_maxRadiusSeenThisTick;

		void broadPhase(std::vector<CollisionCandidate>& outCandidates);

		// TODO: grid-cast footprint overlap + center of mass raycast for multi-part ships,
		// these should be integrated fields from the ship baking process, will test with simple version
		
		[[nodiscard]] bool narrowPhaseSwept(EntityID a, EntityID b, d64 dt) const;
		[[nodiscard]] bool narrowPhaseShip(EntityID a, EntityID b, d64 dt) const;
		[[nodiscard]] void computeImpulse(EntityID a, EntityID b, Vector2float& outImpulseA, Vector2float& outImpulseB) const;
	};
}