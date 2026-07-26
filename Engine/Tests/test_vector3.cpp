#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <Physics/Vector3double.h>
#include <Physics/Vector3float.h>
#include <Core/Logger.h>

using namespace Engine;
using Catch::Matchers::WithinAbs;

namespace {
    struct Vector3LogFixture {
        Logger logger{ Logger::LogLevel::Info };
    };
}

TEST_CASE_METHOD(Vector3LogFixture, "Vector3double::length matches the 3D Pythagorean length", "[Vector3double]") {
    Vector3double v{ 2.0, 3.0, 6.0 };
    d64 len = v.length();
    logger.log(Logger::LogLevel::Info, "Vector3double({}, {}, {}).length() -> {}", v.x, v.y, v.z, len);
    REQUIRE_THAT(len, WithinAbs(7.0, 1e-9)); // sqrt(4+9+36) = 7
}

TEST_CASE_METHOD(Vector3LogFixture, "Vector3double::normalize produces a unit vector", "[Vector3double]") {
    Vector3double v{ 2.0, 3.0, 6.0 };
    v.normalize();
    logger.log(Logger::LogLevel::Info, "Normalized (2,3,6) -> ({}, {}, {}), length {}", v.x, v.y, v.z, v.length());
    REQUIRE_THAT(v.length(), WithinAbs(1.0, 1e-9));
}

TEST_CASE_METHOD(Vector3LogFixture, "Vector3double::dot and cross match hand-computed values", "[Vector3double]") {
    Vector3double a{ 1.0, 0.0, 0.0 };
    Vector3double b{ 0.0, 1.0, 0.0 };
    d64 dot = a.dot(b);
    Vector3double cross = a.cross(b);
    logger.log(Logger::LogLevel::Info, "dot(X,Y) -> {}, cross(X,Y) -> ({}, {}, {})", dot, cross.x, cross.y, cross.z);
    REQUIRE_THAT(dot, WithinAbs(0.0, 1e-9));
    REQUIRE(cross == Vector3double{ 0.0, 0.0, 1.0 }); // X cross Y == Z
}

TEST_CASE_METHOD(Vector3LogFixture, "Vector3double::cross2d matches the z-component of the full 3D cross product", "[Vector3double]") {
    Vector3double a{ 1.0, 2.0, 0.0 };
    Vector3double b{ 3.0, 4.0, 0.0 };
    d64 cross2d = a.cross2d(b);
    logger.log(Logger::LogLevel::Info, "cross2d((1,2),(3,4)) -> {}", cross2d);
    REQUIRE_THAT(cross2d, WithinAbs(-2.0, 1e-9)); // 1*4 - 2*3
}

TEST_CASE_METHOD(Vector3LogFixture, "Vector3double operator[] indexes x, y, z in order and supports mutation", "[Vector3double]") {
    Vector3double v{ 1.0, 2.0, 3.0 };
    logger.log(Logger::LogLevel::Info, "v[0]={} v[1]={} v[2]={}", v[0], v[1], v[2]);
    REQUIRE(v[0] == 1.0);
    REQUIRE(v[1] == 2.0);
    REQUIRE(v[2] == 3.0);

    v[1] = 9.0;
    logger.log(Logger::LogLevel::Info, "After v[1]=9.0 -> ({}, {}, {})", v.x, v.y, v.z);
    REQUIRE(v.y == 9.0);
}

TEST_CASE_METHOD(Vector3LogFixture, "Vector3double arithmetic operators combine components independently", "[Vector3double]") {
    Vector3double a{ 1.0, 2.0, 3.0 };
    Vector3double b{ 4.0, 5.0, 6.0 };

    Vector3double sum = a + b;
    Vector3double diff = b - a;
    Vector3double scaled = a * 2.0;
    logger.log(Logger::LogLevel::Info, "a+b -> ({},{},{}), b-a -> ({},{},{}), a*2 -> ({},{},{})",
        sum.x, sum.y, sum.z, diff.x, diff.y, diff.z, scaled.x, scaled.y, scaled.z);

    REQUIRE(sum == Vector3double{ 5.0, 7.0, 9.0 });
    REQUIRE(diff == Vector3double{ 3.0, 3.0, 3.0 });
    REQUIRE(scaled == Vector3double{ 2.0, 4.0, 6.0 });

    a += b;
    REQUIRE(a == Vector3double{ 5.0, 7.0, 9.0 });
    a -= b;
    REQUIRE(a == Vector3double{ 1.0, 2.0, 3.0 });
    a *= 3.0;
    REQUIRE(a == Vector3double{ 3.0, 6.0, 9.0 });
}

TEST_CASE_METHOD(Vector3LogFixture, "Vector3double has no unexpected padding", "[Vector3double]") {
    logger.log(Logger::LogLevel::Info, "sizeof(Vector3double) -> {} bytes", sizeof(Vector3double));
    REQUIRE(sizeof(Vector3double) == 24);
}

TEST_CASE_METHOD(Vector3LogFixture, "Vector3float::length, dot, and cross match hand-computed values", "[Vector3float]") {
    Vector3float v{ 2.0f, 3.0f, 6.0f };
    f32 len = v.length();
    logger.log(Logger::LogLevel::Info, "Vector3float({}, {}, {}).length() -> {}", v.x, v.y, v.z, len);
    REQUIRE_THAT(len, WithinAbs(7.0f, 1e-4f));

    Vector3float a{ 1.0f, 0.0f, 0.0f };
    Vector3float b{ 0.0f, 1.0f, 0.0f };
    f32 dot = a.dot(b);
    Vector3float cross = a.cross(b);
    logger.log(Logger::LogLevel::Info, "dot(X,Y) -> {}, cross(X,Y) -> ({}, {}, {})", dot, cross.x, cross.y, cross.z);
    REQUIRE_THAT(dot, WithinAbs(0.0f, 1e-6f));
    REQUIRE(cross == Vector3float{ 0.0f, 0.0f, 1.0f }); // X cross Y == Z
}

TEST_CASE_METHOD(Vector3LogFixture, "Vector3float::normalize produces a unit vector", "[Vector3float]") {
    Vector3float v{ 2.0f, 3.0f, 6.0f };
    v.normalize();
    logger.log(Logger::LogLevel::Info, "Normalized (2,3,6) -> ({}, {}, {}), length {}", v.x, v.y, v.z, v.length());
    REQUIRE_THAT(v.length(), WithinAbs(1.0f, 1e-5f));
}

TEST_CASE_METHOD(Vector3LogFixture, "Vector3float::cross2d matches the z-component of the full 3D cross product", "[Vector3float]") {
    Vector3float a{ 1.0f, 2.0f, 0.0f };
    Vector3float b{ 3.0f, 4.0f, 0.0f };
    f32 cross2d = a.cross2d(b);
    logger.log(Logger::LogLevel::Info, "cross2d((1,2),(3,4)) -> {}", cross2d);
    REQUIRE_THAT(cross2d, WithinAbs(-2.0f, 1e-5f)); // 1*4 - 2*3
}

TEST_CASE_METHOD(Vector3LogFixture, "Vector3float arithmetic operators combine components independently", "[Vector3float]") {
    Vector3float a{ 1.0f, 2.0f, 3.0f };
    Vector3float b{ 4.0f, 5.0f, 6.0f };

    Vector3float sum = a + b;
    Vector3float diff = b - a;
    Vector3float scaled = a * 2.0f;
    logger.log(Logger::LogLevel::Info, "a+b -> ({},{},{}), b-a -> ({},{},{}), a*2 -> ({},{},{})",
        sum.x, sum.y, sum.z, diff.x, diff.y, diff.z, scaled.x, scaled.y, scaled.z);

    REQUIRE(sum == Vector3float{ 5.0f, 7.0f, 9.0f });
    REQUIRE(diff == Vector3float{ 3.0f, 3.0f, 3.0f });
    REQUIRE(scaled == Vector3float{ 2.0f, 4.0f, 6.0f });

    a += b;
    REQUIRE(a == Vector3float{ 5.0f, 7.0f, 9.0f });
    a -= b;
    REQUIRE(a == Vector3float{ 1.0f, 2.0f, 3.0f });
    a *= 3.0f;
    REQUIRE(a == Vector3float{ 3.0f, 6.0f, 9.0f });
}

TEST_CASE_METHOD(Vector3LogFixture, "Vector3float has no unexpected padding", "[Vector3float]") {
    logger.log(Logger::LogLevel::Info, "sizeof(Vector3float) -> {} bytes", sizeof(Vector3float));
    REQUIRE(sizeof(Vector3float) == 12);
}
