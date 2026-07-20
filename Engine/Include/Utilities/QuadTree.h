#pragma once
#include <Core/Common.h>
#include <Physics/Vector2double.h>
#include <ECS/EntityRegister.h>
#include <vector>


/// <summary>
/// 2D spatial indexing generic class, rebuilds per-tick in this version. Clear() then insert() for relevent entities each tick.
/// Potential optimization by incredmentally performing updates instead of rebuilds.
/// </summary>
namespace Engine {
	class QuadTree : public Base {
	public:
		explicit QuadTree(const QuadTreeDesc& desc);
		~QuadTree();

		void clear();
		void insert(EntityID id, const Vector2double& position);
		void queryAABB(const Vector2double& min, const Vector2double& max, std::vector<EntityID>& outResults) const;
		void queryRadius(const Vector2double& center, d64 radius, std::vector<EntityID>& outResults) const;

	private:
		struct Node {
			Vector2double min{};
			Vector2double max{};
			std::vector<std::pair<EntityID, Vector2double>> entries{};

			std::unique_ptr<Node> children[4]{};

			[[nodiscard]] bool isLeaf() const noexcept { return children[0] == nullptr; }
		};

		void debugCollectNode(const Node* node, std::vector<Vector2double>& out) const;

		void insertInto(Node* node, EntityID id, const Vector2double& pos, uint8_t depth);
		void subdivide(Node* node, uint8_t depth);
		[[nodiscard]] Node* childFor(Node* node, const Vector2double& pos) const noexcept;

		void queryAABBNode(const Node* node, const Vector2double& qmin, const Vector2double& qmax, std::vector<EntityID>& out) const;
		void queryRadiusNode(const Node* node, const Vector2double& center, d64 radius, const Vector2double& qmin, const Vector2double& qmax, std::vector<EntityID>& out) const;

		Vector2double m_boundsMin{};
		Vector2double m_boundsMax{};
		i32 m_maxDepth{};
		i32 m_maxEntitiesPerNode{};
		std::unique_ptr<Node> m_root{};

	public:
		void debugCollectBounds(std::vector<Vector2double>& outlineVerts) const;
	};

}