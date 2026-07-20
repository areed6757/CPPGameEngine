#include <Physics/QuadTree.h>

Engine::QuadTree::QuadTree(const QuadTreeDesc& desc) : Base(desc.base)
{
}

Engine::QuadTree::~QuadTree()
{
}

void Engine::QuadTree::clear()
{
}

void Engine::QuadTree::insert(EntityID id, const Vector2double& position)
{
}

void Engine::QuadTree::queryAABB(const Vector2double& min, const Vector2double& max, std::vector<EntityID>& outResults) const
{
}

void Engine::QuadTree::queryRadius(const Vector2double& center, d64 radius, std::vector<EntityID>& outResults) const
{
}
