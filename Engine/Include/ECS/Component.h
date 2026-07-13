#pragma once
#include <Core/Common.h>
#include <format>
//#include <cassert>
#include <typeinfo>
#include <Core/Assert.h>

namespace Engine {
	constexpr i32 INVALID_INDEX{ -1 };

	template <typename T>
	class Component : public Base {
	public:
		friend class ECSWrapper;

		explicit Component(const ComponentDesc& desc) : Base(desc.base) {
			m_maxEntities = desc.maxEntities;
			m_sparse.assign(m_maxEntities + 1, INVALID_INDEX);
		}
		~Component() {};
	
	private:
		void add(i32 index, const T& component) {
			if (!(index > 0 && index <= m_maxEntities)) {
				EngineLogError(std::format("Invalid attempt to add component to index {}", index).c_str());
				return;
			}
			if (m_sparse.at(index) != INVALID_INDEX) {
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
			m_sparse.at(index) = INVALID_INDEX;

			EngineLogDebug(std::format("Entity {} destroyed {} component.", index, typeid(T).name()).c_str());
		}

		bool has(i32 index) const noexcept {
			return m_sparse.at(index) != INVALID_INDEX;
		}

		T& get(i32 index) {
			ENGINE_ASSERT(has(index), "get() called on entity with no component of type");
			return m_dense.at(m_sparse.at(index));
		}

		i32 size() {
			return m_dense.size();
		}

		i32 m_maxEntities{};
		std::vector<i32> m_sparse{};
		std::vector<T> m_dense{};
		std::vector<i32> m_backRef{};
	};
}