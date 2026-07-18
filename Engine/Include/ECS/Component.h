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
				EngineLogError(std::format("Invalid attempt to add component to index {}", index).c_str());
				return;
			}
			if (m_sparse.at(index) != INVALID_SENTINEL) {
				EngineLogWarning(std::format("Entity {} already has this component.", index).c_str());
				return;
			}
			m_dense.push_back(component);
			m_backRef.push_back(index);
			m_sparse.at(index) = static_cast<i32> (m_dense.size() - 1);
			EngineLogDebug(std::format("Entity {} added {} component", index, typeid(T).name()).c_str());
		}

		void remove(i32 index) {
			ENGINE_ASSERT(has(index), "remove() called on entity with no component");
			i32 denseIndex = m_sparse.at(index);
			i32 lastDenseIndex = static_cast<i32>(m_dense.size() - 1);
			i32 lastEntity = m_backRef.at(lastDenseIndex);

			m_dense.at(denseIndex) = m_dense.at(lastDenseIndex);
			m_backRef.at(denseIndex) = lastEntity;
			m_sparse.at(lastEntity) = denseIndex;

			m_dense.pop_back();
			m_backRef.pop_back();
			m_sparse.at(index) = INVALID_SENTINEL;

			EngineLogDebug(std::format("Entity {} destroyed {} component.", index, typeid(T).name()).c_str());
		}

		[[nodiscard]] bool has(i32 index) const noexcept {
			if (index < 0 || index >= static_cast<i32>(m_sparse.size())) { return false;  }
			return m_sparse.at(index) != INVALID_SENTINEL;
		}

		[[nodiscard]] T& get(i32 index) {
			ENGINE_ASSERT(has(index), "get() called on entity with no component of type");
			return m_dense.at(m_sparse.at(index));
		}

		[[nodiscard]] T* tryGet(i32 index) noexcept {
			if (index < 0 || index >= static_cast<i32>(m_sparse.size())) { return nullptr; }
			i32 denseIndex = m_sparse[index];
			if (denseIndex == INVALID_SENTINEL) { return nullptr; }
			return &m_dense[denseIndex];
		}

		[[nodiscard]] i32 size() const {
			return static_cast<i32>(m_dense.size());
		}

		[[nodiscard]] i32 entityAt(i32 denseIndex) const {
			return m_backRef.at(denseIndex);
		}

		i32 m_maxEntities{};
		std::vector<i32> m_sparse{};
		std::vector<T> m_dense{};
		std::vector<i32> m_backRef{};
	};
}