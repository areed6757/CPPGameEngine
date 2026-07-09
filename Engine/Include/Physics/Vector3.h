#pragma once
#include <Core/Common.h>

namespace Engine {
	class Vector3 {
	public:
		Vector3();
		Vector3(d64 x, d64 y, d64 z);
		~Vector3();
		
	private:
		d64 m_x{};
		d64 m_y{};
		d64 m_z{};


	};
}