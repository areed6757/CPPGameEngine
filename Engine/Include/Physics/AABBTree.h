#pragma once
#include <Core/Base.h>
#include <Core/Common.h>
#include <Physics/Vector2double.h>
#include <vector>

namespace Engine {
	struct AABB {
		Vector2double min, max;

		[[nodiscard]] bool contains(const AABB& other) const noexcept {
			return min.x <= other.min.x && min.y <= other.min.y &&
				max.x >= other.max.x && max.y >= other.max.y;
		}
		[[nodiscard]] bool overlaps(const AABB& other) const noexcept {
			return min.x <= other.max.x && max.x >= other.min.x &&
				min.y <= other.max.y && max.y >= other.min.y;
		}
		[[nodiscard]] AABB unionWith(const AABB& other) const noexcept {
			return AABB{
				Vector2double{ std::min(min.x, other.min.x), std::min(min.y, other.min.y) },
				Vector2double{ std::max(max.x, other.max.x), std::max(max.y, other.max.y) }
			};
		}
		[[nodiscard]] d64 perimeter() const noexcept {
			return 2.0 * ((max.x - min.x) + (max.y - min.y));
		}
	};

	struct AABBTreeDesc {
		BaseDesc base;
		f32 fatMargin{ 0.1f };     // extra padding (km) so small moves don't trigger a re-insert
		f32 moveMultiplier{ 4.0f }; // predictive AABB extension along velocity direction
	};

	// Dynamic self-balancing binary tree, adapted from Box2D's b2DynamicTree
	// (Erin Catto, zlib license)
	class AABBTree : public Base {
	public:
		explicit AABBTree(const AABBTreeDesc& desc);
		~AABBTree();

		i32 insert(EntityID id, const AABB& bounds);
		void remove(i32 proxyId);

		// Returns true if a real update happened
		bool moveProxy(i32 proxyId, const AABB& newBounds, const Vector2double& displacement);

		void query(const AABB& queryBounds, std::vector<EntityID>& outResults) const;

		[[nodiscard]] EntityID getEntity(i32 proxyId) const noexcept { return m_nodes[proxyId].entity; }
		[[nodiscard]] const AABB& getFatAABB(i32 proxyId) const noexcept { return m_nodes[proxyId].aabb; }

	private:
		struct Node {
			AABB aabb;
			EntityID entity{};
			i32 parent = -1;
			i32 next = -1;
			i32 child1 = -1, child2 = -1;
			i32 height = -1; // -1 = free, 0 = leaf, >0 = internal
			[[nodiscard]] bool isLeaf() const noexcept { return child1 == -1; }
		};

		i32 allocateNode();
		void freeNode(i32 nodeId);
		void insertLeaf(i32 leaf);
		void removeLeaf(i32 leaf);
		i32 balance(i32 iA);
		void queryNode(i32 nodeId, const AABB& queryBounds, std::vector<EntityID>& out) const;

		std::vector<Node> m_nodes;
		i32 m_root = -1;
		i32 m_freeList = -1;
		i32 m_nodeCount = 0;
		f32 m_fatMargin;
		f32 m_moveMultiplier;
	};
}