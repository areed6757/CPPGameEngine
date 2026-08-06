#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <Physics/ORCA.h>
#include <Core/Logger.h>
#include <cmath>

using namespace Engine;
using Catch::Matchers::WithinAbs;

namespace {
    struct ORCAFixture {
        Logger logger{ Logger::LogLevel::Info };
    };

    // A velocity satisfies a line's half-plane constraint when it's on or to the left of it:
    // direction.cross(point - v) <= 0 (up to floating tolerance).
    bool satisfiesLine(const ORCALine& line, const Vector2float& v, f32 tolerance = 1e-3f) {
        return line.direction.cross(line.point - v) <= tolerance;
    }
}

TEST_CASE_METHOD(ORCAFixture, "solveORCA returns the preferred velocity unchanged with no neighbors", "[ORCA]") {
    Vector2float preferred{ 2.0f, 3.0f };
    Vector2float result = solveORCA({}, preferred, 100.0f);
    logger.log(Logger::LogLevel::Info, "No neighbors: preferred ({}, {}) -> result ({}, {})", preferred.x, preferred.y, result.x, result.y);
    REQUIRE_THAT(result.x, WithinAbs(preferred.x, 1e-4f));
    REQUIRE_THAT(result.y, WithinAbs(preferred.y, 1e-4f));
}

TEST_CASE_METHOD(ORCAFixture, "solveORCA clamps the preferred velocity to maxSpeed with no neighbors", "[ORCA]") {
    Vector2float preferred{ 30.0f, 40.0f }; // length 50
    Vector2float result = solveORCA({}, preferred, 10.0f);
    logger.log(Logger::LogLevel::Info, "Over maxSpeed: preferred length {} clamped -> result ({}, {}), length {}", preferred.length(), result.x, result.y, result.length());
    REQUIRE_THAT(result.length(), WithinAbs(10.0f, 1e-3f));
    REQUIRE_THAT(result.x / result.length(), WithinAbs(preferred.x / preferred.length(), 1e-4f));
    REQUIRE_THAT(result.y / result.length(), WithinAbs(preferred.y / preferred.length(), 1e-4f));
}

TEST_CASE_METHOD(ORCAFixture, "computeORCALine on a non-colliding pair produces a unit-length direction", "[ORCA]") {
    ORCALine line = computeORCALine(Vector2float{ 1.0f, 0.0f }, Vector2float{ 10.0f, 3.0f }, Vector2float{ -1.0f, 0.0f }, 1.0f, 5.0f, 0.1f);
    logger.log(Logger::LogLevel::Info, "Line direction ({}, {}), length {}", line.direction.x, line.direction.y, line.direction.length());
    REQUIRE_THAT(line.direction.length(), WithinAbs(1.0f, 1e-3f));
}

TEST_CASE_METHOD(ORCAFixture, "solveORCA leaves velocity unchanged when neighbor is on a clearly non-colliding course", "[ORCA]") {
    // Neighbor is 100 units to the right; both ships fly straight up/down (perpendicular), never converging.
    Vector2float selfVelocity{ 0.0f, 5.0f };
    Vector2float neighborVelocity{ 0.0f, -5.0f };
    ORCALine line = computeORCALine(selfVelocity, Vector2float{ 100.0f, 0.0f }, neighborVelocity, 1.0f, 3.0f, 0.1f);

    Vector2float result = solveORCA({ line }, selfVelocity, 100.0f);
    logger.log(Logger::LogLevel::Info, "Non-colliding pair: preferred ({}, {}) -> result ({}, {})", selfVelocity.x, selfVelocity.y, result.x, result.y);
    REQUIRE_THAT(result.x, WithinAbs(selfVelocity.x, 1e-2f));
    REQUIRE_THAT(result.y, WithinAbs(selfVelocity.y, 1e-2f));
}

TEST_CASE_METHOD(ORCAFixture, "solveORCA steers a head-on closing course away from the raw preferred velocity", "[ORCA]") {
    // Neighbor 10 units away (slightly off-axis to avoid the exact-symmetry tie case), both closing
    // head-on at combined speed 10 - well inside combinedRadius=2 within timeHorizon=5.
    Vector2float selfVelocity{ 5.0f, 0.0f };
    Vector2float neighborVelocity{ -5.0f, 0.0f };
    Vector2float relativePosition{ 10.0f, 0.5f };
    ORCALine line = computeORCALine(selfVelocity, relativePosition, neighborVelocity, 2.0f, 5.0f, 0.1f);

    Vector2float result = solveORCA({ line }, selfVelocity, 100.0f);
    f32 deviation = (result - selfVelocity).length();
    logger.log(Logger::LogLevel::Info, "Head-on course: preferred ({}, {}) -> result ({}, {}), deviation {}", selfVelocity.x, selfVelocity.y, result.x, result.y, deviation);

    REQUIRE(deviation > 0.5f); // avoidance actually changed course, not a no-op
    REQUIRE(satisfiesLine(line, result));
    REQUIRE(result.length() <= 100.0f + 1e-3f);
}

TEST_CASE_METHOD(ORCAFixture, "computeORCALine on an already-overlapping pair pushes straight apart", "[ORCA]") {
    // Neighbor is 0.5 units away (inside combinedRadius=2.0, i.e. already overlapping), both stationary.
    ORCALine line = computeORCALine(Vector2float{ 0.0f, 0.0f }, Vector2float{ 0.5f, 0.0f }, Vector2float{ 0.0f, 0.0f }, 2.0f, 3.0f, 0.1f);
    Vector2float result = solveORCA({ line }, Vector2float{ 0.0f, 0.0f }, 100.0f);
    logger.log(Logger::LogLevel::Info, "Overlapping pair: result ({}, {})", result.x, result.y);

    // Hand-derived exact expected value for this input (invTimeStep=10, w=(-5,0), unitW=(-1,0),
    // u=(-1,0)*(2*10-5)=(-15,0), line.point=(-7.5,0), direction=(0,1) -> nearest feasible point to
    // the origin on that vertical line is (-7.5, 0), i.e. straight away from the neighbor.
    REQUIRE_THAT(result.x, WithinAbs(-7.5f, 1e-2f));
    REQUIRE_THAT(result.y, WithinAbs(0.0f, 1e-2f));
}

TEST_CASE_METHOD(ORCAFixture, "computeORCALine cap-branch case matches hand-derived exact constraint", "[ORCA]") {
    // Neighbor 10 units to the right, both stationary, combinedRadius=1, timeHorizon=5 -> falls on
    // the truncation-circle cap branch. Hand-derived: line direction (0,1), point (0.9, 0), so the
    // constraint is v.x <= 0.9; projecting a preferred velocity of (5,0) onto it lands at (0.9, 0).
    ORCALine line = computeORCALine(Vector2float{ 0.0f, 0.0f }, Vector2float{ 10.0f, 0.0f }, Vector2float{ 0.0f, 0.0f }, 1.0f, 5.0f, 0.1f);
    Vector2float result = solveORCA({ line }, Vector2float{ 5.0f, 0.0f }, 100.0f);
    logger.log(Logger::LogLevel::Info, "Cap-branch case: line point ({}, {}), direction ({}, {}) -> result ({}, {})",
        line.point.x, line.point.y, line.direction.x, line.direction.y, result.x, result.y);

    REQUIRE_THAT(line.point.x, WithinAbs(0.9f, 1e-2f));
    REQUIRE_THAT(result.x, WithinAbs(0.9f, 1e-2f));
    REQUIRE_THAT(result.y, WithinAbs(0.0f, 1e-2f));
}

TEST_CASE_METHOD(ORCAFixture, "computeORCALine's selfResponsibility controls how much this agent gives way", "[ORCA]") {
    // Same head-on course as the earlier avoidance test, but swept across responsibility 0 (self is
    // immovable, e.g. a station), 0.5 (equal-mass reciprocal split), and 1 (self dodges alone, e.g.
    // avoiding a station) to check the mass-driven split actually changes how much self moves.
    Vector2float selfVelocity{ 5.0f, 0.0f };
    Vector2float neighborVelocity{ -5.0f, 0.0f };
    Vector2float relativePosition{ 10.0f, 0.5f };

    ORCALine immovableSelf = computeORCALine(selfVelocity, relativePosition, neighborVelocity, 2.0f, 5.0f, 0.1f, 0.0f);
    ORCALine sharedResponsibility = computeORCALine(selfVelocity, relativePosition, neighborVelocity, 2.0f, 5.0f, 0.1f, 0.5f);
    ORCALine fullResponsibility = computeORCALine(selfVelocity, relativePosition, neighborVelocity, 2.0f, 5.0f, 0.1f, 1.0f);

    f32 deviationImmovable = (solveORCA({ immovableSelf }, selfVelocity, 100.0f) - selfVelocity).length();
    f32 deviationShared = (solveORCA({ sharedResponsibility }, selfVelocity, 100.0f) - selfVelocity).length();
    f32 deviationFull = (solveORCA({ fullResponsibility }, selfVelocity, 100.0f) - selfVelocity).length();
    logger.log(Logger::LogLevel::Info, "Deviation by responsibility: immovable {}, shared {}, full {}", deviationImmovable, deviationShared, deviationFull);

    REQUIRE_THAT(deviationImmovable, WithinAbs(0.0f, 1e-3f)); // an immovable self never gets pushed off its own preferred velocity
    REQUIRE(deviationFull > deviationShared);
    REQUIRE(deviationShared > deviationImmovable);
}

TEST_CASE_METHOD(ORCAFixture, "solveORCA falls back to a finite least-violation velocity when constraints are jointly infeasible", "[ORCA]") {
    // Two constraints that can never both be satisfied (v.x <= -1 and v.x >= 1 simultaneously) -
    // exercises the linearProgram3 fallback path rather than a clean linearProgram2 solve.
    ORCALine line1{ Vector2float{ -1.0f, 0.0f }, Vector2float{ 0.0f, 1.0f } };  // v.x <= -1
    ORCALine line2{ Vector2float{ 1.0f, 0.0f }, Vector2float{ 0.0f, -1.0f } }; // v.x >= 1

    Vector2float result = solveORCA({ line1, line2 }, Vector2float{ 0.0f, 0.0f }, 100.0f);
    logger.log(Logger::LogLevel::Info, "Infeasible constraints: fallback result ({}, {})", result.x, result.y);

    REQUIRE(std::isfinite(result.x));
    REQUIRE(std::isfinite(result.y));
    REQUIRE(result.length() <= 100.0f + 1e-3f);
}
