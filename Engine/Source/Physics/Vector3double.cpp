#include <Physics/Vector3double.h>

Engine::d64 Engine::Vector3double::length() const noexcept
{
	return d64(sqrt(x*x + y*y + z*z));
}

Engine::Vector3double& Engine::Vector3double::normalize() noexcept
{
	d64 len = length();
	if (len > 0.0) {
		x /= len;
		y /= len;
		z /= len;
	}
	return *this;
}

Engine::d64 Engine::Vector3double::dot(const Vector3double& v) const noexcept
{
	return (x * v.x + y * v.y + z * v.z);
}

Engine::Vector3double Engine::Vector3double::cross(const Vector3double& v) const noexcept
{
	return {
		y*v.z - z*v.y,
		z*v.x - x*v.z,
		x*v.y - y*v.x
	};
}

Engine::d64 Engine::Vector3double::cross2d(const Vector3double& v) const noexcept {
	return { x * v.y - y * v.x };
}

Engine::Vector3double& Engine::Vector3double::operator+=(const Vector3double& v) noexcept
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

Engine::Vector3double Engine::Vector3double::operator+(const Vector3double& v) const noexcept
{
	return Vector3double{ x+v.x, y+v.y, z+v.z};
}

Engine::Vector3double& Engine::Vector3double::operator-=(const Vector3double& v) noexcept
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

Engine::Vector3double Engine::Vector3double::operator-(const Vector3double& v) const noexcept
{
	return Vector3double{ x - v.x, y - v.y, z - v.z};
}

Engine::Vector3double& Engine::Vector3double::operator*=(d64 s) noexcept
{
	x *= s;
	y *= s;
	z *= s;
	return *this;
}

Engine::Vector3double Engine::Vector3double::operator*(d64 s) const noexcept
{
	return Vector3double{x*s, y*s, z*s};
}
