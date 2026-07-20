#include <Utilities/QuadTree.h>

namespace Engine {

	QuadTree::QuadTree(const QuadTreeDesc& desc) : Base(desc.base),
		m_boundsMin(desc.boundsMin),
		m_boundsMax(desc.boundsMax),
		m_maxDepth(desc.maxDepth),
		m_maxEntitiesPerNode(desc.maxEntitiesPerNode)
	{
		clear();
		EngineLogInfo("Quadtree created.");
	}

	QuadTree::~QuadTree()
	{
		EngineLogInfo("Quadtree destroyed.");
	}

	void QuadTree::clear()
	{
		m_root = std::make_unique<Node>();
		m_root->min = m_boundsMin;
		m_root->max = m_boundsMax;
	}

	void QuadTree::insert(EntityID id, const Vector2double& position)
	{
		insertInto(m_root.get(), id, position, 0);
	}

	void QuadTree::insertInto(Node* node, EntityID id, const Vector2double& pos, uint8_t depth)
	{
		if (!node->isLeaf()) {
			insertInto(childFor(node, pos), id, pos, depth + 1);
			return;
		}

		node->entries.emplace_back(id, pos);

		if (static_cast<uint8_t>(node->entries.size()) > m_maxEntitiesPerNode && depth < m_maxDepth) {
			subdivide(node, depth);
		}
	}

	void QuadTree::subdivide(Node* node, uint8_t depth)
	{
		Vector2double mid{
			(node->min.x + node->max.x) / 2.0,
			(node->min.y + node->max.y) / 2.0,
		};

		node->children[0] = std::make_unique<Node>();	// bottom-left
		node->children[0]->min = node->min;
		node->children[0]->max = mid;

		node->children[1] = std::make_unique<Node>(); // bottom-right
		node->children[1]->min = Vector2double{ mid.x, node->min.y };
		node->children[1]->max = Vector2double{ node->max.x, mid.y };

		node->children[2] = std::make_unique<Node>(); // top-left
		node->children[2]->min = Vector2double{ node->min.x, mid.y };
		node->children[2]->max = Vector2double{ mid.x, node->max.y };

		node->children[3] = std::make_unique<Node>(); // top-right
		node->children[3]->min = mid;
		node->children[3]->max = node->max;

		auto movedEntries = std::move(node->entries);
		node->entries.clear();
		node->entries.shrink_to_fit();

		for (auto& [entryId, entryPos] : movedEntries) {
			insertInto(childFor(node, entryPos), entryId, entryPos, depth + 1);
		}
	}

	QuadTree::Node* QuadTree::childFor(Node* node, const Vector2double& pos) const noexcept
	{
		Vector2double mid{
			(node->min.x + node->max.x) / 2.0,
			(node->min.y + node->max.y) / 2.0
		};

		bool right = pos.x >= mid.x;
		bool top = pos.y >= mid.y;

		if (!right && !top) { return node->children[0].get(); }
		if (right && !top) { return node->children[1].get(); }
		if (!right && top) { return node->children[2].get(); }
		return node->children[3].get();
	}

	void QuadTree::queryAABB(const Vector2double& min, const Vector2double& max, std::vector<EntityID>& outResults) const
	{
		queryAABBNode(m_root.get(), min, max, outResults);
	}

	void QuadTree::queryAABBNode(const Node* node, const Vector2double& qmin, const Vector2double& qmax, std::vector<EntityID>& out) const
	{
		if (!node) { return; }

		if (node->max.x < qmin.x || node->min.x > qmax.x || node->max.y < qmin.y || node->min.y > qmax.y) { return; }

		if (node->isLeaf()) {
			for (const auto& [id, pos] : node->entries) {
				if (pos.x >= qmin.x && pos.x <= qmax.x && pos.y >= qmin.y && pos.y <= qmax.y) { out.push_back(id); }
			}
			return;
		}

		for (const auto& child : node->children) {
			queryAABBNode(child.get(), qmin, qmax, out);
		}
	}

	void QuadTree::queryRadius(const Vector2double& center, d64 radius, std::vector<EntityID>& outResults) const
	{
		Vector2double qmin{ center.x - radius, center.y - radius };
		Vector2double qmax{ center.x + radius, center.y + radius };
		queryRadiusNode(m_root.get(), center, radius, qmin, qmax, outResults);
	}

	void QuadTree::queryRadiusNode(const Node* node, const Vector2double& center, d64 radius, const Vector2double& qmin, const Vector2double& qmax, std::vector<EntityID>& out) const
	{
		if (!node) { return; }

		if (node->max.x < qmin.x || node->min.x > qmax.x || node->max.y < qmin.y || node->min.y > qmax.y) { return; }

		if (node->isLeaf()) {
			d64 r2 = radius * radius;
			for (const auto& [id, pos] : node->entries) {
				d64 dx = pos.x - center.x;
				d64 dy = pos.y - center.y;
				if (dx * dx + dy * dy <= r2) {
					out.push_back(id);
				}
			}

			return;
		}

		for (const auto& child : node->children) {
			queryRadiusNode(child.get(), center, radius, qmin, qmax, out);
		}
	}

	void QuadTree::debugCollectBounds(std::vector<Vector2double>& outlineVerts) const {
		debugCollectNode(m_root.get(), outlineVerts);
	}

	void QuadTree::debugCollectNode(const Node* node, std::vector<Vector2double>& out) const {
		if (!node) { return; }

		out.push_back({ node->min.x, node->min.y }); out.push_back({ node->max.x, node->min.y });
		out.push_back({ node->max.x, node->min.y }); out.push_back({ node->max.x, node->max.y });
		out.push_back({ node->max.x, node->max.y }); out.push_back({ node->min.x, node->max.y });
		out.push_back({ node->min.x, node->max.y }); out.push_back({ node->min.x, node->min.y });

		for (const auto& child : node->children) {
			debugCollectNode(child.get(), out);
		}
	}
}