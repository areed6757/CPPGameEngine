#pragma once
#include <Core/Common.h>
#include <cmath>

namespace Engine {
    struct Vector2double {
        d64 x{};
        d64 y{};

        // Allow float -> double widening on explicit calls to do so
        // This will allow velocities to be stored in floats, but update double positions
        explicit Vector2double(const Vector2float& v) noexcept : x(v.x), y(v.y) {}

        Vector2double() = default;
        Vector2double(d64 x_, d64 y_) noexcept : x(x_), y(y_) {}


        [[nodiscard]] d64 length() const noexcept { return d64(sqrt(x * x + y * y)); }

        Vector2double& normalize() noexcept {
            d64 len = length();
            if (len > 0.0f) { x /= len; y /= len; }
            return *this;
        }

        [[nodiscard]] d64 dot(const Vector2double& v) const noexcept { return x * v.x + y * v.y; }
        [[nodiscard]] d64 cross(const Vector2double& v) const noexcept { return x * v.y - y * v.x; }

        Vector2double& operator+=(const Vector2double& v) noexcept { x += v.x; y += v.y; return *this; }
        [[nodiscard]] Vector2double operator+(const Vector2double& v) const noexcept { return { x + v.x, y + v.y }; }
        Vector2double& operator-=(const Vector2double& v) noexcept { x -= v.x; y -= v.y; return *this; }
        [[nodiscard]] Vector2double operator-(const Vector2double& v) const noexcept { return { x - v.x, y - v.y }; }
        Vector2double& operator*=(d64 s) noexcept { x *= s; y *= s; return *this; }
        [[nodiscard]] Vector2double operator*(d64 s) const noexcept { return { x * s, y * s }; }
    };
}