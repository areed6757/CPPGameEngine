#include <Physics/Vector3float.h>

Engine::f32 Engine::Vector3float::length() const noexcept
{
	return f32(sqrt(x * x + y * y + z * z));
}

Engine::Vector3float& Engine::Vector3float::normalize() noexcept
{
	f32 len = length();
	if (len > 0.0) {
		x /= len;
		y /= len;
		z /= len;
	}
	return *this;
}

Engine::f32 Engine::Vector3float::dot(const Vector3float& v) const noexcept
{
	return (x * v.x + y * v.y + z * v.z);
}

Engine::Vector3float Engine::Vector3float::cross(const Vector3float& v) const noexcept
{
	return {
		y * v.z - z * v.y,
		z * v.x - x * v.z,
		x * v.y - y * v.x
	};
}

Engine::f32 Engine::Vector3float::cross2d(const Vector3float& v) const noexcept {
	return { x * v.y - y * v.x };
}

Engine::Vector3float& Engine::Vector3float::operator+=(const Vector3float& v) noexcept
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

Engine::Vector3float Engine::Vector3float::operator+(const Vector3float& v) const noexcept
{
	return Vector3float{ x + v.x, y + v.y, z + v.z };
}

Engine::Vector3float& Engine::Vector3float::operator-=(const Vector3float& v) noexcept
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

Engine::Vector3float Engine::Vector3float::operator-(const Vector3float& v) const noexcept
{
	return Vector3float{ x - v.x, y - v.y, z - v.z };
}

Engine::Vector3float& Engine::Vector3float::operator*=(f32 s) noexcept
{
	x *= s;
	y *= s;
	z *= s;
	return *this;
}

Engine::Vector3float Engine::Vector3float::operator*(f32 s) const noexcept
{
	return Vector3float{ x * s, y * s, z * s };
}
