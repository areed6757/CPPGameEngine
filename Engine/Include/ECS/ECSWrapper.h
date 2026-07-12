#pragma once
#include <Core/Common.h>
#include <ECS/EntityRegister.h>
#include <ECS/Component.h>
#include <ECS/Components/Transform.h>
#include <ECS/Components/Movement.h>

/// <summary>
/// This is a wrapper class to contain all public functions of the ECS package. It will hold a reference to a created EntityRegister as well
/// as the logic for efficiently handling creation and deletion of both entities and components. All ticked systems will interface with this class
/// per their own automated loops that are called in update() cycles.
/// 
/// NOTE: (for now) When adding a new Component type, #include the file, add it to the ComponentPools struct and initialize it there.
/// </summary>
namespace Engine {
	struct ComponentPools {
		Component<Transform> transforms;
		Component<Movement> movements;

		explicit ComponentPools(const ComponentDesc& desc) : transforms(desc), movements(desc) {}
	};

	class ECSWrapper : public Base {
	public:
		explicit ECSWrapper(const ECSWrapperDesc& desc);
		~ECSWrapper();

		/*
		* TODO: 
		* Create bitset for easy entity type identification
		* Resolve TODOs below
		*/

		EntityID createEntity() { return m_entityReg.create(); }
		void destroyEntity(EntityID id) { m_entityReg.destroy(id); } // TODO: Destroy all associated components as well
		bool isValidEntity(EntityID id) { return m_entityReg.isValid(id); }

		// Called as hasComponent<T>(EntityID);
		template <typename T>
		bool hasComponent(EntityID id) {
			if (!isValidEntity(id)) { return false; } // TODO: handle fail

			if constexpr (std::is_same_v<T, Transform>) {
				return m_pools.transforms.has(id.id);
			}
			else if constexpr (std::is_same_v<T, Movement>) {
				return m_pools.movements.has(id.id);
			}
			else {
				static_assert(sizeof(T) == 0, "hasComponent: unregistered component type");
			}
		}

		// Called as addComponent<T>(EntityID);
		template <typename T>
		void addComponent(EntityID id, T& component) {
			if (!isValidEntity(id)) { return; } // TODO: handle fail

			if constexpr (std::is_same_v<T, Transform>) {
				m_pools.transforms.add(id.id, component);
			}
			else if constexpr (std::is_same_v<T, Movement>) {
				m_pools.movements.add(id.id, component);
			}
			else {
				static_assert(sizeof(T) == 0, "addComponent: unregistered component type");
			}
		}

		// Called as removeComponent<T>(EntityID);
		template <typename T>
		void removeComponent(EntityID id) {
			if (!isValidEntity(id)) { return; } // TODO: handle fail

			if constexpr (std::is_same_v<T, Transform>) {
				m_pools.transforms.remove(id.id);
			}
			else if constexpr (std::is_same_v<T, Movement>) {
				m_pools.movements.remove(id.id);
			}
			else {
				static_assert(sizeof(T) == 0, "removeComponent: unregistered component type");
			}
		}

		// Called as getComponent<T>(EntityID);
		template <typename T>
		T& getComponent(EntityID id) {
			if constexpr (std::is_same_v<T, Transform>) {
				return m_pools.transforms.get(id.id);
			}
			else if constexpr (std::is_same_v<T, Movement>) {
				return m_pools.movements.get(id.id);
			}
			else {
				static_assert(sizeof(T) == 0, "getComponent: unregistered component type");
			}
		}

		// Called as sizeComponentPool<T>();
		template <typename T>
		i32 sizeComponentPool() {
			if constexpr (std::is_same_v<T, Transform>) {
				return m_pools.transforms.size();
			}
			else if constexpr (std::is_same_v<T, Movement>) {
				return m_pools.movements.size();
			}
			else {
				static_assert(sizeof(T) == 0, "getComponent: unregistered component type");
			}
		}

	private:
		EntityRegister& m_entityReg;
		ComponentPools m_pools;
	};
}