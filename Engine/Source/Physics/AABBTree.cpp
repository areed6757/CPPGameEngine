#include <Physics/AABBTree.h>
#include <algorithm>
#include <cmath>

namespace Engine {
	AABBTree::AABBTree(const AABBTreeDesc& desc) : Base(desc.base),
		m_fatMargin(desc.fatMargin), m_moveMultiplier(desc.moveMultiplier)
	{
		i32 initialCapacity = 16;
		m_nodes.resize(initialCapacity);
		for (i32 i = 0; i < initialCapacity - 1; i++) { m_nodes[i].next = i + 1; }
		m_nodes[initialCapacity - 1].next = -1;
		m_freeList = 0;
		EngineLogInfo("AABBTree created.");
	}

	AABBTree::~AABBTree()
	{
		EngineLogInfo("AABBTree destroyed.");
	}

	i32 AABBTree::allocateNode()
	{
		if (m_freeList == -1) {
			// Pool exhausted, grow it, relink the new tail as free nodes.
			i32 oldCapacity = static_cast<i32>(m_nodes.size());
			i32 newCapacity = oldCapacity * 2;
			m_nodes.resize(newCapacity);
			for (i32 i = oldCapacity; i < newCapacity - 1; i++) { m_nodes[i].next = i + 1; }
			m_nodes[newCapacity - 1].next = -1;
			m_freeList = oldCapacity;
		}

		i32 nodeId = m_freeList;
		m_freeList = m_nodes[nodeId].next;
		m_nodes[nodeId] = Node{};
		m_nodes[nodeId].height = 0;
		m_nodeCount++;
		return nodeId;
	}

	void AABBTree::freeNode(i32 nodeId)
	{
		m_nodes[nodeId].next = m_freeList;
		m_nodes[nodeId].height = -1;
		m_freeList = nodeId;
		m_nodeCount--;
	}

	i32 AABBTree::insert(EntityID id, const AABB& bounds)
	{
		i32 proxyId = allocateNode();

		Vector2double margin{ static_cast<d64>(m_fatMargin), static_cast<d64>(m_fatMargin) };
		m_nodes[proxyId].aabb = AABB{ bounds.min - margin, bounds.max + margin };
		m_nodes[proxyId].entity = id;
		m_nodes[proxyId].height = 0;

		insertLeaf(proxyId);
		return proxyId;
	}

	void AABBTree::remove(i32 proxyId)
	{
		removeLeaf(proxyId);
		freeNode(proxyId);
	}

	bool AABBTree::moveProxy(i32 proxyId, const AABB& newBounds, const Vector2double& displacement)
	{
		if (m_nodes[proxyId].aabb.contains(newBounds)) { return false; } // still within the fattened AABB, no update needed

		removeLeaf(proxyId);

		Vector2double margin{ static_cast<d64>(m_fatMargin), static_cast<d64>(m_fatMargin) };
		AABB fattened{ newBounds.min - margin, newBounds.max + margin };

		// Predictive extension along the direction of travel, so a fast-
		// moving entity's fat AABB reaches ahead of it
		Vector2double d{ static_cast<d64>(m_moveMultiplier) * displacement.x, static_cast<d64>(m_moveMultiplier) * displacement.y };
		if (d.x < 0.0) { fattened.min.x += d.x; }
		else { fattened.max.x += d.x; }
		if (d.y < 0.0) { fattened.min.y += d.y; }
		else { fattened.max.y += d.y; }

		m_nodes[proxyId].aabb = fattened;
		insertLeaf(proxyId);
		return true;
	}

	void AABBTree::insertLeaf(i32 leaf)
	{
		if (m_root == -1) {
			m_root = leaf;
			m_nodes[m_root].parent = -1;
			return;
		}

		AABB leafAABB = m_nodes[leaf].aabb;
		i32 index = m_root;

		while (!m_nodes[index].isLeaf()) {
			i32 child1 = m_nodes[index].child1;
			i32 child2 = m_nodes[index].child2;

			d64 area = m_nodes[index].aabb.perimeter();
			AABB combined = m_nodes[index].aabb.unionWith(leafAABB);
			d64 combinedArea = combined.perimeter();

			d64 cost = 2.0 * combinedArea;
			d64 inheritanceCost = 2.0 * (combinedArea - area);

			d64 cost1;
			if (m_nodes[child1].isLeaf()) {
				cost1 = leafAABB.unionWith(m_nodes[child1].aabb).perimeter() + inheritanceCost;
			}
			else {
				d64 oldArea = m_nodes[child1].aabb.perimeter();
				d64 newArea = leafAABB.unionWith(m_nodes[child1].aabb).perimeter();
				cost1 = (newArea - oldArea) + inheritanceCost;
			}

			d64 cost2;
			if (m_nodes[child2].isLeaf()) {
				cost2 = leafAABB.unionWith(m_nodes[child2].aabb).perimeter() + inheritanceCost;
			}
			else {
				d64 oldArea = m_nodes[child2].aabb.perimeter();
				d64 newArea = leafAABB.unionWith(m_nodes[child2].aabb).perimeter();
				cost2 = (newArea - oldArea) + inheritanceCost;
			}

			if (cost < cost1 && cost < cost2) { break; }
			index = (cost1 < cost2) ? child1 : child2;
		}

		i32 sibling = index;
		i32 oldParent = m_nodes[sibling].parent;
		i32 newParent = allocateNode();

		m_nodes[newParent].parent = oldParent;
		m_nodes[newParent].aabb = leafAABB.unionWith(m_nodes[sibling].aabb);
		m_nodes[newParent].height = m_nodes[sibling].height + 1;

		if (oldParent != -1) {
			if (m_nodes[oldParent].child1 == sibling) { m_nodes[oldParent].child1 = newParent; }
			else { m_nodes[oldParent].child2 = newParent; }
		}
		else {
			m_root = newParent;
		}
		m_nodes[newParent].child1 = sibling;
		m_nodes[newParent].child2 = leaf;
		m_nodes[sibling].parent = newParent;
		m_nodes[leaf].parent = newParent;

		// Walk back to the root, rebalancing and refitting AABBs/heights
		index = m_nodes[leaf].parent;
		while (index != -1) {
			index = balance(index);
			i32 child1 = m_nodes[index].child1;
			i32 child2 = m_nodes[index].child2;
			m_nodes[index].height = 1 + std::max(m_nodes[child1].height, m_nodes[child2].height);
			m_nodes[index].aabb = m_nodes[child1].aabb.unionWith(m_nodes[child2].aabb);
			index = m_nodes[index].parent;
		}
	}

	void AABBTree::removeLeaf(i32 leaf)
	{
		if (leaf == m_root) {
			m_root = -1;
			return;
		}

		i32 parent = m_nodes[leaf].parent;
		i32 grandParent = m_nodes[parent].parent;
		i32 sibling = (m_nodes[parent].child1 == leaf) ? m_nodes[parent].child2 : m_nodes[parent].child1;

		if (grandParent != -1) {
			if (m_nodes[grandParent].child1 == parent) { m_nodes[grandParent].child1 = sibling; }
			else { m_nodes[grandParent].child2 = sibling; }
			m_nodes[sibling].parent = grandParent;
			freeNode(parent);

			i32 index = grandParent;
			while (index != -1) {
				index = balance(index);
				i32 child1 = m_nodes[index].child1;
				i32 child2 = m_nodes[index].child2;
				m_nodes[index].aabb = m_nodes[child1].aabb.unionWith(m_nodes[child2].aabb);
				m_nodes[index].height = 1 + std::max(m_nodes[child1].height, m_nodes[child2].height);
				index = m_nodes[index].parent;
			}
		}
		else {
			m_root = sibling;
			m_nodes[sibling].parent = -1;
			freeNode(parent);
		}
	}

	// AVL-style rotation. If node A's subtrees differ in height by more than
	// 1, rotates the taller child up to restore balance. Returns the new
	// local root of this subtree (may be unchanged).
	i32 AABBTree::balance(i32 iA)
	{
		Node& A = m_nodes[iA];
		if (A.isLeaf() || A.height < 2) { return iA; }

		i32 iB = A.child1, iC = A.child2;
		Node& B = m_nodes[iB];
		Node& C = m_nodes[iC];

		i32 heightDiff = C.height - B.height;

		if (heightDiff > 1) {
			i32 iF = C.child1, iG = C.child2;
			Node& F = m_nodes[iF];
			Node& G = m_nodes[iG];

			C.child1 = iA;
			C.parent = A.parent;
			A.parent = iC;

			if (C.parent != -1) {
				if (m_nodes[C.parent].child1 == iA) { m_nodes[C.parent].child1 = iC; }
				else { m_nodes[C.parent].child2 = iC; }
			}
			else {
				m_root = iC;
			}

			if (F.height > G.height) {
				C.child2 = iF;
				A.child2 = iG;
				G.parent = iA;
				A.aabb = B.aabb.unionWith(G.aabb);
				C.aabb = A.aabb.unionWith(F.aabb);
				A.height = 1 + std::max(B.height, G.height);
				C.height = 1 + std::max(A.height, F.height);
			}
			else {
				C.child2 = iG;
				A.child2 = iF;
				F.parent = iA;
				A.aabb = B.aabb.unionWith(F.aabb);
				C.aabb = A.aabb.unionWith(G.aabb);
				A.height = 1 + std::max(B.height, F.height);
				C.height = 1 + std::max(A.height, G.height);
			}
			return iC;
		}

		if (heightDiff < -1) {
			i32 iD = B.child1, iE = B.child2;
			Node& D = m_nodes[iD];
			Node& E = m_nodes[iE];

			B.child1 = iA;
			B.parent = A.parent;
			A.parent = iB;

			if (B.parent != -1) {
				if (m_nodes[B.parent].child1 == iA) { m_nodes[B.parent].child1 = iB; }
				else { m_nodes[B.parent].child2 = iB; }
			}
			else {
				m_root = iB;
			}

			if (D.height > E.height) {
				B.child2 = iD;
				A.child1 = iE;
				E.parent = iA;
				A.aabb = C.aabb.unionWith(E.aabb);
				B.aabb = A.aabb.unionWith(D.aabb);
				A.height = 1 + std::max(C.height, E.height);
				B.height = 1 + std::max(A.height, D.height);
			}
			else {
				B.child2 = iE;
				A.child1 = iD;
				D.parent = iA;
				A.aabb = C.aabb.unionWith(D.aabb);
				B.aabb = A.aabb.unionWith(E.aabb);
				A.height = 1 + std::max(C.height, D.height);
				B.height = 1 + std::max(A.height, E.height);
			}
			return iB;
		}

		return iA;
	}

	void AABBTree::query(const AABB& queryBounds, std::vector<EntityID>& outResults) const
	{
		outResults.clear();
		if (m_root != -1) { queryNode(m_root, queryBounds, outResults); }
	}

	void AABBTree::queryNode(i32 nodeId, const AABB& queryBounds, std::vector<EntityID>& out) const
	{
		if (nodeId == -1) { return; }
		if (!m_nodes[nodeId].aabb.overlaps(queryBounds)) { return; }

		if (m_nodes[nodeId].isLeaf()) {
			out.push_back(m_nodes[nodeId].entity);
			return;
		}
		queryNode(m_nodes[nodeId].child1, queryBounds, out);
		queryNode(m_nodes[nodeId].child2, queryBounds, out);
	}
}