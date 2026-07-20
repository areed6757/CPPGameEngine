#pragma once
#include <Core/Common.h>
#include <Physics/Vector2double.h>
#include <ECS/EntityRegister.h>
#include <vector>

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
		// node structure?
	};

}