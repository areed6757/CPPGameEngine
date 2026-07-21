#pragma once
#include <Core/Assert.h>
#include <Core/Common.h>
#include <format>
#include <typeinfo>

/// <summary>
///		Component<T&> is sparse/dense set storage pool for a single component type T.
///	A sparse set lookup array maps an entity index directly to its position in the dense
///	array, the dense array is a memory-contiguous storage for entities that hold this component,
///	and m_backRef maps a dense position back to the entity that owns it for swap/pop removal.
/// 
///		All mutating access (add/remove) and read access (has/get/size/entityAt) is private and
///	reachable only through ECSWrapper friend class. No other classes should ever
///	touch Component pools directly. This ensures entity signatures and component storage
///	are guaranteed to be in-sync.
/// 
///		T is expected to be a plain, copyable data struct, no logic or inheritance. This
///	follows a strict ECS implementation plan, components are pure data, systems perform logic.
/// </summary>
namespace Engine {
	template <typename T>
	class Component : public Base {
	public:

		explicit Component(const ComponentDesc& desc) : Base(desc.base) {
			m_maxEntities = desc.maxEntities;
			m_sparse.assign(static_cast<size_t>(m_maxEntities) + 1, INVALID_SENTINEL);
		}
		~Component() {};


		void add(i32 index, const T& component) {
			if (!(index > 0 && index <= m_maxEntities)) {
				EngineLogError("Invalid attempt to add component to index {}", index);
				return;
			}
			if (m_sparse[index] != INVALID_SENTINEL) {
				EngineLogWarning("Entity {} already has this component.", index);
				return;
			}
			m_dense.push_back(component);
			m_backRef.push_back(index);
			m_sparse[index] = static_cast<i32> (m_dense.size() - 1);
			EngineLogDebug("Entity {} added {} component", index, typeid(T).name());
		}

		void remove(i32 index) {
			ENGINE_ASSERT(has(index), "remove() called on entity with no component");
			i32 denseIndex = m_sparse[index];
			i32 lastDenseIndex = static_cast<i32>(m_dense.size() - 1);
			i32 lastEntity = m_backRef[lastDenseIndex];

			m_dense[denseIndex] = m_dense[lastDenseIndex];
			m_backRef[denseIndex] = lastEntity;
			m_sparse[lastEntity] = denseIndex;

			m_dense.pop_back();
			m_backRef.pop_back();
			m_sparse[index] = INVALID_SENTINEL;

			EngineLogDebug("Entity {} destroyed {} component.", index, typeid(T).name());
		}

		[[nodiscard]] bool has(i32 index) const noexcept {
			if (index < 0 || index >= static_cast<i32>(m_sparse.size())) { return false;  }
			return m_sparse[index] != INVALID_SENTINEL;
		}

		[[nodiscard]] T& get(i32 index) {
			ENGINE_ASSERT(has(index), "get() called on entity with no component of type");
			return m_dense[m_sparse[index]];
		}

		[[nodiscard]] T* tryGet(i32 index) noexcept {
			if (index < 0 || index >= static_cast<i32>(m_sparse.size())) { return nullptr; }
			i32 denseIndex = m_sparse[index];
			if (denseIndex == INVALID_SENTINEL) { return nullptr; }
			return &m_dense[denseIndex];
		}

		[[nodiscard]] T& getAtDenseIndex(i32 denseIndex) {
			ENGINE_ASSERT(denseIndex >= 0 && denseIndex < static_cast<i32>(m_dense.size()), "getAtDenseIndex: index out of range");
			return m_dense[denseIndex];
		}

		[[nodiscard]] i32 size() const {
			return static_cast<i32>(m_dense.size());
		}

		[[nodiscard]] i32 entityAt(i32 denseIndex) const {
			ENGINE_ASSERT(denseIndex >= 0 && denseIndex < static_cast<i32>(m_backRef.size()), "entityAt: index out of range");
			return m_backRef[denseIndex];
		}

		i32 m_maxEntities{};
		std::vector<i32> m_sparse{};
		std::vector<T> m_dense{};
		std::vector<i32> m_backRef{};
	};
}