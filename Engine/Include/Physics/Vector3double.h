#pragma once
#include <Core/Common.h>
#include <cmath>

// Vector3 with no Z-axis controls, for 2D implementation in 3D space. Holds 3 doubles.
namespace Engine {
    struct Vector3double {
        d64 x{}, y{}, z{};

        d64& operator[](int i) noexcept { return (&x)[i]; }
        d64 operator[](int i) const noexcept { return (&x)[i]; }

        d64 length() const noexcept;
        Vector3double& normalize() noexcept;
        d64 dot(const Vector3double& v) const noexcept;
        Vector3double cross(const Vector3double& v) const noexcept;
        d64 cross2d(const Vector3double& v) const noexcept; // Used for 2d plane cross products where z = 0 for 3d vector locked to the 2d plane

        bool operator==(const Vector3double& v) const noexcept = default;

        Vector3double& operator+=(const Vector3double& v) noexcept;
        Vector3double operator+(const Vector3double& v) const noexcept;
        Vector3double& operator-=(const Vector3double& v) noexcept;
        Vector3double operator-(const Vector3double& v) const noexcept;
        Vector3double& operator*=(d64 s) noexcept;
        Vector3double operator*(d64 s) const noexcept;
    };

    static_assert(sizeof(Vector3double) == 24, "Vector3double has unexpected padding");
}