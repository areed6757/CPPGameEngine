#pragma once
#include <Core/Common.h>
#include <cmath>

namespace Engine {
    struct Vector2float {
        f32 x{};
        f32 y{};

        [[nodiscard]] f32 length() const noexcept { return f32(sqrt(x * x + y * y)); }

        Vector2float& normalize() noexcept {
            f32 len = length();
            if (len > 0.0) { x /= len; y /= len; }
            return *this;
        }

        [[nodiscard]] f32 dot(const Vector2float& v) const noexcept { return x * v.x + y * v.y; }
        [[nodiscard]] f32 cross(const Vector2float& v) const noexcept { return x * v.y - y * v.x; }

        Vector2float& operator+=(const Vector2float& v) noexcept { x += v.x; y += v.y; return *this; }
        [[nodiscard]] Vector2float operator+(const Vector2float& v) const noexcept { return { x + v.x, y + v.y }; }
        Vector2float& operator-=(const Vector2float& v) noexcept { x -= v.x; y -= v.y; return *this; }
        [[nodiscard]] Vector2float operator-(const Vector2float& v) const noexcept { return { x - v.x, y - v.y }; }
        Vector2float& operator*=(f32 s) noexcept { x *= s; y *= s; return *this; }
        [[nodiscard]] Vector2float operator*(f32 s) const noexcept { return { x * s, y * s }; }
    };
}