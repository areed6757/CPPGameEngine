#pragma once
#include <Core/Common.h>

/// <summary>
/// EntityRegister assigns entityIDs automatically based on the currently available IDs in the pool. When an Entity is created, a paired integer to the ID named "generation"
/// will be incremented so that processes that utilize entityIDs will be able to update their current targets. (Reassigning a unique entityID without generations would cause 
/// some functions to continue targetting the destroyed Entity)
/// </summary>
namespace Engine {
	struct EntityID{
		i32 index{};
		i32 generation{};

		auto operator<=>(const EntityID&) const = default; // Defaults all six comparison operators
	};

	class EntityRegister : public Base {
	public:
		explicit EntityRegister(const EntityRegisterDesc& desc);
		~EntityRegister();

		EntityID create();
		void destroy(EntityID& id);
		bool isValid(EntityID& id);

	private:
		i32 m_maxEntities{};
		std::vector<i32> m_generations;
		std::vector<i32> m_freeIndices;
	};
}