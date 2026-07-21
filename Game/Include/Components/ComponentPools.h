#pragma once
#include <Core/Common.h>
#include <ECS/Component.h>
#include <ECS/ECSWrapper.h>
#include <Components/Movement.h>
#include <Components/Position.h>
#include <Components/Renderable.h>
#include <Components/Physics.h>
#include <Components/DamagePayload.h>
#include <Components/Health.h>
#include <Components/Lifetime.h>
#include <Components/Thruster.h>
#include <Components/Weapon.h>

/// <summary>
/// This class holds pools of components as the single source of access to all components types by all systems in the game.
/// Adding a component requires 5 updates in this file only.
///		1. #include
///		2. ComponentBit
///		3/4. getPool and const getPool overload method
///		5. removeAll
/// No other files necessarily require an update for a new component type unless they seek to use that component.
/// </summary>
namespace Engine {
	template <> struct ComponentBit<Position> { static constexpr i32 value = 0; };
	template <> struct ComponentBit<Movement> { static constexpr i32 value = 1; };
	template <> struct ComponentBit<Renderable> { static constexpr i32 value = 2; };
	template <> struct ComponentBit<Physics> { static constexpr i32 value = 3; };
	template <> struct ComponentBit<DamagePayload> { static constexpr i32 value = 4; };
	template <> struct ComponentBit<Health> { static constexpr i32 value = 5; };
	template <> struct ComponentBit<Lifetime> { static constexpr i32 value = 6; };
	template <> struct ComponentBit<Thruster> { static constexpr i32 value = 7; };
	template <> struct ComponentBit<Weapon> { static constexpr i32 value = 8; };

	struct ComponentPools {
		explicit ComponentPools(const ComponentDesc& desc) : positions(desc), movements(desc), renderables(desc), physics(desc) {}

	private:
		Component<Position> positions;
		Component<Movement> movements;
		Component<Renderable> renderables;
		Component<Physics> physics;
		Component<DamagePayload> damagePayloads;
		Component<Health> healths;
		Component<Lifetime> lifetimes;
		Component<Thruster> thrusters;
		Component<Weapon> weapons;

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
			else if constexpr (std::is_same_v<T, Renderable>) return renderables;
			else if constexpr (std::is_same_v<T, Physics>) return physics;
			else if constexpr (std::is_same_v<T, DamagePayload>) return damagePayloads;
			else if constexpr (std::is_same_v<T, Health>) return healths;
			else if constexpr (std::is_same_v<T, Lifetime>) return lifetimes;
			else if constexpr (std::is_same_v<T, Thruster>) return thrusters;
			else if constexpr (std::is_same_v<T, Weapon>) return weapons;
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
			else if constexpr (std::is_same_v<T, Renderable>) return renderables;
			else if constexpr (std::is_same_v<T, Physics>) return physics;
			else if constexpr (std::is_same_v<T, DamagePayload>) return damagePayloads;
			else if constexpr (std::is_same_v<T, Health>) return healths;
			else if constexpr (std::is_same_v<T, Lifetime>) return lifetimes;
			else if constexpr (std::is_same_v<T, Thruster>) return thrusters;
			else if constexpr (std::is_same_v<T, Weapon>) return weapons;
			else static_assert(sizeof(T) == 0, "getPool: unregistered component type");
		}

		void removeAll(i32 index) {
			if (positions.has(index)) { positions.remove(index); }
			if (movements.has(index)) { movements.remove(index); }
			if (renderables.has(index)) { renderables.remove(index); }
			if (physics.has(index)) { physics.remove(index); }
			if (damagePayloads.has(index)) { damagePayloads.remove(index); }
			if (healths.has(index)) { healths.remove(index); }
			if (lifetimes.has(index)) { lifetimes.remove(index); }
			if (thrusters.has(index)) { thrusters.remove(index); }
			if (weapons.has(index)) { weapons.remove(index); }
		}
	};

}