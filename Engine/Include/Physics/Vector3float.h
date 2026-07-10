#pragma once
#include <Core/Common.h>
#include <cmath>

// Vector3 with no Z-axis controls, for 2D implementation in 3D space. Holds 3 floats.
namespace Engine {
    struct Vector3float {
        f32 x{}, y{}, z{};

        f32& operator[](int i) noexcept { return (&x)[i]; }
        f32 operator[](int i) const noexcept { return (&x)[i]; }

        f32 length() const noexcept;
        Vector3float& normalize() noexcept;
        f32 dot(const Vector3float& v) const noexcept;
        Vector3float cross(const Vector3float& v) const noexcept;
        f32 cross2d(const Vector3float& v) const noexcept; // Used for 2d plane cross products where z = 0 for 3d vector locked to the 2d plane

        bool operator==(const Vector3float& v) const noexcept = default;

        Vector3float& operator+=(const Vector3float& v) noexcept;
        Vector3float operator+(const Vector3float& v) const noexcept;
        Vector3float& operator-=(const Vector3float& v) noexcept;
        Vector3float operator-(const Vector3float& v) const noexcept;
        Vector3float& operator*=(f32 s) noexcept;
        Vector3float operator*(f32 s) const noexcept;
    };

    static_assert(sizeof(Vector3float) == 12, "Vector3float has unexpected padding");
}