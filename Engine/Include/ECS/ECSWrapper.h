#pragma once
#include <bitset>
#include <vector>
#include <format>
#include <Core/Common.h>
#include <ECS/EntityRegister.h>
#include <ECS/Component.h>
#include <ECS/Components/Position.h>
#include <ECS/Components/Movement.h>

/// <summary>
/// This is a wrapper class to contain all public functions of the ECS package. It will hold a reference to a created EntityRegister as well
/// as the logic for efficiently handling creation and deletion of both entities and components. All ticked systems will interface with this class
/// per their own automated loops that are called in update() cycles.
/// 
/// NOTE: (for now) When adding a new Component type, #include the file, add it to the ComponentPools struct and initialize it there, add it to getPool(), and create a 
///	ComponentBit struct in the template list below.
/// 
/// Entity signatures are generated in the form of a bitset so that external systems updating per tick can lookup a signature set instead of parsing
/// every component on every entity to find entities that require their update service. i.e. Movement and Position are required components for the
/// MovementSystem to update an entity, it will simply look for bitset signatures *11 then update matches.
/// </summary>
namespace Engine {
	struct ComponentPools {
		Component<Position> transforms;
		Component<Movement> movements;

		explicit ComponentPools(const ComponentDesc& desc) : transforms(desc), movements(desc) {}
	};


	// Register Component bits for entity bitset tracking
	template <typename T>
	struct ComponentBit;

	template <> struct ComponentBit<Position> { static constexpr i32 value = 0; };
	template <> struct ComponentBit<Movement> { static constexpr i32 value = 1; };


	class ECSWrapper : public Base {
	private:
		EntityRegister& m_entityReg;
		ComponentPools m_pools;
		std::vector<std::bitset<64>> m_entitySignatures{};

		template<typename T>
		Component<T>& getPool() {
			if constexpr (std::is_same_v<T, Position>) {
				return m_pools.transforms;
			}
			else if constexpr (std::is_same_v<T, Movement>) {
				return m_pools.movements;
			}
			else {
				static_assert(sizeof(T) == 0, "getPool: unregistered component type");
			}
		}

	public:
		explicit ECSWrapper(const ECSWrapperDesc& desc);
		~ECSWrapper();

		EntityID createEntity() { return m_entityReg.create(); }
		bool isValidEntity(EntityID id) { return m_entityReg.isValid(id); }
		
		void destroyEntity(EntityID id) {
			if (!isValidEntity(id)) {
				EngineLogDebug(std::format("Attempted to destroy invalid entity id: {} generation: {}", id.id, id.generation).c_str());
				return;
			}
			
			if (m_pools.transforms.has(id.id)) { removeComponent<Position>(id); }
			if (m_pools.movements.has(id.id)) { removeComponent<Movement>(id); }

			m_entitySignatures.at(id.id).reset(); // This is redundant after calling each removeComponent, but it doesn't hurt
			m_entityReg.destroy(id); 
		}

		// Called as hasComponent<T>(EntityID);
		template <typename T>
		bool hasComponent(EntityID id) {
			if (!isValidEntity(id)) { return false; } // TODO: handle fail
			return getPool<T>().has(id.id);
		}

		// Called as addComponent<T>(EntityID);
		template <typename T>
		void addComponent(EntityID id, const T& component) {
			if (!isValidEntity(id)) { return; } // TODO: handle fail
			getPool<T>().add(id.id, component);
			m_entitySignatures.at(id.id).set(ComponentBit<T>::value);
		}

		// Called as removeComponent<T>(EntityID);
		template <typename T>
		void removeComponent(EntityID id) {
			if (!isValidEntity(id)) { return; } // TODO: handle fail
			getPool<T>().remove(id.id);
			m_entitySignatures.at(id.id).reset(ComponentBit<T>::value);
		}

		// Called as getComponent<T>(EntityID);
		template <typename T>
		T& getComponent(EntityID id) {
			return getPool<T>().get(id.id);
		}

		// Called as sizeComponentPool<T>();
		template <typename T>
		i32 sizeComponentPool() {
			return getPool<T>().size();
		}

		// Used by Systems to parse a component dense list for entity updates
		template <typename T>
		i32 entityAtDenseIndex(i32 denseIndex) {
			return getPool<T>().entityAt(denseIndex);
		}

		EntityID entityFromIndex(i32 index) {
			return EntityID{ index, m_entityReg.generationAt(index) };
		}

		// Used to create a unique signature for calling update Systems as: m_bitMask(ECSWrapper(instance).makeSignature<Position, Movement>())	
		template <typename... Ts>
		std::bitset<64> makeSignature() {
			std::bitset<64> mask;
			((mask.set(ComponentBit<Ts>::value)), ...);
			return mask;
		}

		std::bitset<64> getSignature(EntityID id) const {
			return m_entitySignatures.at(id.id);
		}


	};
}