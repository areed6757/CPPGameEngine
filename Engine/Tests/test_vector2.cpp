#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <Physics/Vector2double.h>
#include <Physics/Vector2float.h>
#include <Core/Logger.h>

using namespace Engine;
using Catch::Matchers::WithinAbs;

namespace {
    struct Vector2LogFixture {
        Logger logger{ Logger::LogLevel::Info };
    };
}

TEST_CASE_METHOD(Vector2LogFixture, "Vector2double::length matches the Pythagorean length of its components", "[Vector2double]") {
    Vector2double v{ 3.0, 4.0 };
    d64 len = v.length();
    logger.log(Logger::LogLevel::Info, "Vector2double({}, {}).length() -> {}", v.x, v.y, len);
    REQUIRE_THAT(len, WithinAbs(5.0, 1e-9));
}

TEST_CASE_METHOD(Vector2LogFixture, "Vector2double::normalize produces a unit vector pointing the same direction", "[Vector2double]") {
    Vector2double v{ 3.0, 4.0 };
    v.normalize();
    logger.log(Logger::LogLevel::Info, "Normalized (3, 4) -> ({}, {}), length {}", v.x, v.y, v.length());
    REQUIRE_THAT(v.length(), WithinAbs(1.0, 1e-9));
    REQUIRE_THAT(v.x, WithinAbs(0.6, 1e-9));
    REQUIRE_THAT(v.y, WithinAbs(0.8, 1e-9));
}

TEST_CASE_METHOD(Vector2LogFixture, "Vector2double::normalize on a zero vector leaves it untouched rather than dividing by zero", "[Vector2double]") {
    Vector2double v{ 0.0, 0.0 };
    v.normalize();
    logger.log(Logger::LogLevel::Info, "Normalizing zero vector -> ({}, {}) (guarded, no NaN expected)", v.x, v.y);
    REQUIRE(v.x == 0.0);
    REQUIRE(v.y == 0.0);
}

TEST_CASE_METHOD(Vector2LogFixture, "Vector2double::dot and cross match hand-computed values", "[Vector2double]") {
    Vector2double a{ 1.0, 2.0 };
    Vector2double b{ 3.0, 4.0 };
    d64 dot = a.dot(b);
    d64 cross = a.cross(b);
    logger.log(Logger::LogLevel::Info, "dot((1,2),(3,4)) -> {}, cross((1,2),(3,4)) -> {}", dot, cross);
    REQUIRE_THAT(dot, WithinAbs(11.0, 1e-9));   // 1*3 + 2*4
    REQUIRE_THAT(cross, WithinAbs(-2.0, 1e-9)); // 1*4 - 2*3
}

TEST_CASE_METHOD(Vector2LogFixture, "Vector2double arithmetic operators combine components independently", "[Vector2double]") {
    Vector2double a{ 1.0, 2.0 };
    Vector2double b{ 3.0, 4.0 };

    Vector2double sum = a + b;
    Vector2double diff = b - a;
    Vector2double scaled = a * 2.0;
    logger.log(Logger::LogLevel::Info, "a+b -> ({}, {}), b-a -> ({}, {}), a*2 -> ({}, {})",
        sum.x, sum.y, diff.x, diff.y, scaled.x, scaled.y);

    REQUIRE(sum.x == 4.0); REQUIRE(sum.y == 6.0);
    REQUIRE(diff.x == 2.0); REQUIRE(diff.y == 2.0);
    REQUIRE(scaled.x == 2.0); REQUIRE(scaled.y == 4.0);

    a += b;
    REQUIRE(a.x == 4.0); REQUIRE(a.y == 6.0);
    a -= b;
    REQUIRE(a.x == 1.0); REQUIRE(a.y == 2.0);
    a *= 3.0;
    REQUIRE(a.x == 3.0); REQUIRE(a.y == 6.0);
}

TEST_CASE_METHOD(Vector2LogFixture, "Vector2double widens explicitly from a Vector2float without truncation", "[Vector2double]") {
    Vector2float f{ 1.5f, -2.5f };
    Vector2double d{ f };
    logger.log(Logger::LogLevel::Info, "Widened Vector2float({}, {}) -> Vector2double({}, {})", f.x, f.y, d.x, d.y);
    REQUIRE_THAT(d.x, WithinAbs(1.5, 1e-6));
    REQUIRE_THAT(d.y, WithinAbs(-2.5, 1e-6));
}

TEST_CASE_METHOD(Vector2LogFixture, "Vector2float::length, dot, and cross match hand-computed values", "[Vector2float]") {
    Vector2float v{ 3.0f, 4.0f };
    f32 len = v.length();
    logger.log(Logger::LogLevel::Info, "Vector2float({}, {}).length() -> {}", v.x, v.y, len);
    REQUIRE_THAT(len, WithinAbs(5.0f, 1e-5f));

    Vector2float a{ 1.0f, 2.0f };
    Vector2float b{ 3.0f, 4.0f };
    logger.log(Logger::LogLevel::Info, "dot((1,2),(3,4)) -> {}, cross((1,2),(3,4)) -> {}", a.dot(b), a.cross(b));
    REQUIRE_THAT(a.dot(b), WithinAbs(11.0f, 1e-5f));
    REQUIRE_THAT(a.cross(b), WithinAbs(-2.0f, 1e-5f));
}

TEST_CASE_METHOD(Vector2LogFixture, "Vector2float arithmetic operators combine components independently", "[Vector2float]") {
    Vector2float a{ 1.0f, 2.0f };
    Vector2float b{ 3.0f, 4.0f };

    Vector2float sum = a + b;
    Vector2float scaled = b * 2.0f;
    logger.log(Logger::LogLevel::Info, "a+b -> ({}, {}), b*2 -> ({}, {})", sum.x, sum.y, scaled.x, scaled.y);

    REQUIRE(sum.x == 4.0f); REQUIRE(sum.y == 6.0f);
    REQUIRE(scaled.x == 6.0f); REQUIRE(scaled.y == 8.0f);

    b -= a;
    REQUIRE(b.x == 2.0f); REQUIRE(b.y == 2.0f);
}
