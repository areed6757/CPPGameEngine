#pragma once
#include <Core/Common.h>

// Vector3 with no Z-axis controls, for 2D implementation in 3D space.
namespace Engine {
	class Vector3 {
	public:
		Vector3();
		Vector3(d64 x, d64 y);
		~Vector3();
		
	private:
		d64 m_x{};
		d64 m_y{};
		d64 m_z{};


	};
}