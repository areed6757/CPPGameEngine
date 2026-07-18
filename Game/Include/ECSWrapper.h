#pragma once
#include <Components/Position.h>
#include <Components/Movement.h>
#include <Core/Common.h>
#include <ECS/Component.h>
#include <ECS/EntityRegister.h>
#include <bitset>
#include <vector>
#include <format>

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
/// 
/// TODO:
///		This class should be stripped of concrete components by creating a type-erased interface
///		Following the above, move ECSWrapper files into Engine directory
/// </summary>
namespace Engine {

	/// <summary>
	/// Owns one Component<T&> pool per registered component type. This is the one place in the
	///		engine that names every concrete component type explicitly, ref. to class-level notes above
	///		for what touching a new component type requires.
	/// </summary>
	struct ComponentPools {
		Component<Position> positions;
		Component<Movement> movements;
		Component<Renderable> renders;

		explicit ComponentPools(const ComponentDesc& desc) : positions(desc), movements(desc) {}
	};


	/// <summary>
	/// Maps a component type to a fixed bit index within an entity's 64-bit signature. No default
	///		definition is provided deliverately, using an unregistered type here fails to compile
	///		in place of picking a dummy bit. This is intended.
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template <typename T>
	struct ComponentBit;

	template <> struct ComponentBit<Position> { static constexpr i32 value = 0; };
	template <> struct ComponentBit<Movement> { static constexpr i32 value = 1; };
	template <> struct ComponentBit<Renderable> { static constexpr i32 value = 2; };

	class ECSWrapper : public Base {
	private:
		EntityRegister& m_entityReg;
		ComponentPools m_pools;
		std::vector<std::bitset<64>> m_entitySignatures{}; // Per-entity component signature

		/// <summary>
		/// Mutable pool access used by addComponent/ removeComponent
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <returns></returns>
		template<typename T>
		Component<T>& getPool() {
			if constexpr (std::is_same_v<T, Position>) return m_pools.positions;
			else if constexpr (std::is_same_v<T, Movement>) return m_pools.movements;
			else static_assert(sizeof(T) == 0, "getPool: unregistered component type");
		}

		/// <summary>
		/// Read-only pool access, used by hasComponent, sizeComponentPool, entityAtDenseIndex,
		///		called from const methods
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <returns></returns>
		template<typename T>
		const Component<T>& getPool() const {
			if constexpr (std::is_same_v<T, Position>) return m_pools.positions;
			else if constexpr (std::is_same_v<T, Movement>) return m_pools.movements;
			else static_assert(sizeof(T) == 0, "getPool: unregistered component type");
		}

	public:
		explicit ECSWrapper(const ECSWrapperDesc& desc);
		~ECSWrapper();

		/// <summary>
		/// Allocates a fresh entity id from the underlying EntityRegister.
		/// </summary>
		/// <returns></returns>
		[[nodiscard]] EntityID createEntity() { return m_entityReg.create(); }
		
		/// <summary>
		/// True if id refers to a currently-live entity (index and generation match)
		/// </summary>
		/// <param name="id"></param>
		/// <returns></returns>
		[[nodiscard]] bool isValidEntity(EntityID id) const { return m_entityReg.isValid(id); }
		
		/// <summary>
		/// Removes every component the entity currently holds, clears its signature, then recycles
		///		the entity id via EntityRegister. Components must be removed while the id is still valid, so
		///		destruction order matters: components first, then id last.
		/// </summary>
		/// <param name="id"></param>
		void destroyEntity(EntityID id) {
			if (!isValidEntity(id)) {
				EngineLogDebug(std::format("Attempted to destroy invalid entity id: {} generation: {}", id.id, id.generation).c_str());
				return;
			}
			
			if (m_pools.positions.has(id.id)) { removeComponent<Position>(id); }
			if (m_pools.movements.has(id.id)) { removeComponent<Movement>(id); }

			m_entityReg.destroy(id); 
		}

		/// <summary>
		/// True if entity id currently has a component of type T. Called as:
		///		hasComponent<T&>(id)
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="id"></param>
		/// <returns></returns>
		template <typename T>
		[[nodiscard]] bool hasComponent(EntityID id) const {
			if (!isValidEntity(id)) { return false; } // TODO: handle fail
			return getPool<T>().has(id.id);
		}

		/// <summary>
		/// Attaches a copy of component to entity id and sets the corresdponding signature bit.
		///		Called as: addComponent<T&>(id, <Some_Component>{...})
		///		T resolves from the argument
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="id"></param>
		/// <param name="component"></param>
		template <typename T>
		void addComponent(EntityID id, const T& component) {
			if (!isValidEntity(id)) { return; } // TODO: handle fail
			getPool<T>().add(id.id, component);
			m_entitySignatures.at(id.id).set(ComponentBit<T>::value);
		}

		/// <summary>
		/// Removes entity id's component of type T and clears the corresponding signature bit. Called as:
		///		removeComponent<T&>(id)
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="id"></param>
		template <typename T>
		void removeComponent(EntityID id) {
			if (!isValidEntity(id)) { return; } // TODO: handle fail
			getPool<T>().remove(id.id);
			m_entitySignatures.at(id.id).reset(ComponentBit<T>::value);
		}

		/// <summary>
		/// Returns a mutable reference to entity id's component of type T.
		///		Caller should validate hasComponent<T&>(id) first.
		///		Called as getComponent<T&>(id)
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="id"></param>
		/// <returns></returns>
		template <typename T>
		[[nodiscard]] T& getComponent(EntityID id) {
			return getPool<T>().get(id.id);
		}

		/// <summary>
		/// Number of entities currently holding component type T. Called as:
		///		sizeComponentPool<T&>()
		/// Should be used to select smallest pool to parse using a bitset signature for updates.
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <returns></returns>
		template <typename T>
		[[nodiscard]] i32 sizeComponentPool() const noexcept {
			return getPool<T>().size();
		}

		/// <summary>
		/// Given a position in compoennt type T's dense array, returns the owning entity's raw index.
		///		Used by systems doing a signature-driven iteration over a component pool's dense array rather
		///		than scanning every possible entity index. Paired with entityFromInex() to get a full EntityID.
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="denseIndex"></param>
		/// <returns></returns>
		template <typename T>
		[[nodiscard]] i32 entityAtDenseIndex(i32 denseIndex) const {
			return getPool<T>().entityAt(denseIndex);
		}

		/// <summary>
		/// Reconstructs a full EntityID (index + generation) from a raw entity index.
		/// </summary>
		/// <param name="index"></param>
		/// <returns></returns>
		[[nodiscard]] EntityID entityFromIndex(i32 index) const {
			return EntityID{ index, m_entityReg.generationAt(index) };
		}

		/// <summary>
		/// Builds a query bitmask for the given component types, e.g. makeSignature<Position, Movement>()
		///		Compare against getSignature(id) with a bitwise AND to test whether an entity has at least
		///		all of the requested component types (superset match, not exact).
		/// </summary>
		/// <typeparam name="...Ts"></typeparam>
		/// <returns></returns>
		template <typename... Ts>
		[[nodiscard]] std::bitset<64> makeSignature() const {
			std::bitset<64> mask;
			((mask.set(ComponentBit<Ts>::value)), ...);
			return mask;
		}

		/// <summary>
		/// Returns entity id's current component signature bitset.
		/// </summary>
		/// <param name="id"></param>
		/// <returns></returns>
		[[nodiscard]] std::bitset<64> getSignature(EntityID id) const {
			return m_entitySignatures.at(id.id);
		}


	};
}