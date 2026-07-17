#pragma once
#include <Core/Common.h>
#include <ECS/Component.h>
#include <ECS/ECSWrapper.h>
#include <Components/Movement.h>
#include <Components/Position.h>

namespace Engine {
	template <> struct ComponentBit<Position> { static constexpr i32 value = 0; };
	template <> struct ComponentBit<Movement> { static constexpr i32 value = 1; };

	struct ComponentPools {
		explicit ComponentPools(const ComponentDesc& desc) : positions(desc), movements(desc) {}

	private:
		Component<Position> positions;
		Component<Movement> movements;

	public:
		/// <summary>
		/// Mutable pool access used by addComponent/ removeComponent
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <returns></returns>
		template<typename T>
		Component<T>& getPool() {
			if constexpr (std::is_same_v<T, Position>) return positions;
			else if constexpr (std::is_same_v<T, Movement>) return movements;
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
			if constexpr (std::is_same_v<T, Position>) return positions;
			else if constexpr (std::is_same_v<T, Movement>) return movements;
			else static_assert(sizeof(T) == 0, "getPool: unregistered component type");
		}

		void removeAll(i32 index) {
			if (positions.has(index)) { positions.remove(index); }
			if (movements.has(index)) { movements.remove(index); }
		}
	};

}