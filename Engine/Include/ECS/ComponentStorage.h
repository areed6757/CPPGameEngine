#pragma once
#include <Core/Common.h>

namespace Engine {
	constexpr i32 INVALID_INDEX{ -1 };

	template <typename T>
	class ComponentStorage : public Base {
	public:
		explicit ComponentStorage(const ComponentStorageDesc& desc);
		~ComponentStorage();

		void add(i32 index, const T& value) {

		}

		void remove(i32 index) {

		}

		bool has(i32 index) const noexcept {

		}

		T& get(i32 index) {

		}
	
	private:
		std::vector<i32> m_sparse;
		std::vector<T> m_dense;
		std::vector<i32> m_backRef;
	};
}