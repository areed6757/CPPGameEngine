#include <Physics/ORCA.h>
#include <algorithm>
#include <cmath>

namespace Engine {
	namespace {
		constexpr f32 ORCA_EPSILON = 1e-5f;

		// point on lines[lineNo] satisfying lines[0..lineNo) within maxSpeed, closest to optVelocity, or furthest along direction if directionOpt
		bool linearProgram1(const std::vector<ORCALine>& lines, size_t lineNo, f32 maxSpeed,
			const Vector2float& optVelocity, bool directionOpt, Vector2float& result)
		{
			f32 dotProduct = lines[lineNo].point.dot(lines[lineNo].direction);
			f32 discriminant = dotProduct * dotProduct + maxSpeed * maxSpeed - lines[lineNo].point.dot(lines[lineNo].point);
			if (discriminant < 0.0f) { return false; } // line lies entirely outside the maxSpeed circle

			f32 sqrtDiscriminant = std::sqrt(discriminant);
			f32 tLeft = -dotProduct - sqrtDiscriminant;
			f32 tRight = -dotProduct + sqrtDiscriminant;

			for (size_t i = 0; i < lineNo; i++) {
				f32 denominator = lines[lineNo].direction.cross(lines[i].direction);
				f32 numerator = lines[i].direction.cross(lines[lineNo].point - lines[i].point);

				if (std::abs(denominator) <= ORCA_EPSILON) {
					// parallel lines, either always satisfied or never, no new bound on t
					if (numerator < 0.0f) { return false; }
					continue;
				}

				f32 t = numerator / denominator;
				if (denominator >= 0.0f) { tRight = std::min(tRight, t); }
				else { tLeft = std::max(tLeft, t); }

				if (tLeft > tRight) { return false; }
			}

			if (directionOpt) {
				f32 t = optVelocity.dot(lines[lineNo].direction) > 0.0f ? tRight : tLeft;
				result = lines[lineNo].point + lines[lineNo].direction * t;
			}
			else {
				f32 t = lines[lineNo].direction.dot(optVelocity - lines[lineNo].point);
				t = std::clamp(t, tLeft, tRight);
				result = lines[lineNo].point + lines[lineNo].direction * t;
			}
			return true;
		}

		// clips optVelocity against every line, returns lines.size() on success or the first infeasible line's index
		size_t linearProgram2(const std::vector<ORCALine>& lines, f32 maxSpeed, const Vector2float& optVelocity,
			bool directionOpt, Vector2float& result)
		{
			if (directionOpt) {
				result = optVelocity * maxSpeed;
			}
			else if (optVelocity.dot(optVelocity) > maxSpeed * maxSpeed) {
				result = optVelocity;
				result.normalize();
				result *= maxSpeed;
			}
			else {
				result = optVelocity;
			}

			for (size_t i = 0; i < lines.size(); i++) {
				if (lines[i].direction.cross(lines[i].point - result) > 0.0f) {
					Vector2float tempResult = result;
					if (!linearProgram1(lines, i, maxSpeed, optVelocity, directionOpt, result)) {
						result = tempResult;
						return i;
					}
				}
			}
			return lines.size();
		}

		// fallback when constraints are jointly infeasible, minimizes the worst violation starting from the line linearProgram2 failed on
		void linearProgram3(const std::vector<ORCALine>& lines, size_t beginLine, f32 maxSpeed, Vector2float& result)
		{
			f32 distance = 0.0f;
			for (size_t i = beginLine; i < lines.size(); i++) {
				if (lines[i].direction.cross(lines[i].point - result) > distance) {
					// result violates line i more than anything seen so far, re-solve using only the lines bounding line i, projected onto it
					std::vector<ORCALine> projLines(lines.begin(), lines.begin() + static_cast<std::ptrdiff_t>(i));

					for (size_t j = 0; j < i; j++) {
						ORCALine line;
						f32 determinant = lines[i].direction.cross(lines[j].direction);

						if (std::abs(determinant) <= ORCA_EPSILON) {
							if (lines[i].direction.dot(lines[j].direction) > 0.0f) { continue; } // redundant, same side
							line.point = (lines[i].point + lines[j].point) * 0.5f;
						}
						else {
							line.point = lines[i].point + lines[i].direction *
								(lines[j].direction.cross(lines[i].point - lines[j].point) / determinant);
						}

						line.direction = lines[j].direction - lines[i].direction;
						line.direction.normalize();
						projLines.push_back(line);
					}

					Vector2float tempResult = result;
					Vector2float perpendicular{ -lines[i].direction.y, lines[i].direction.x };
					if (linearProgram2(projLines, maxSpeed, perpendicular, true, result) < projLines.size()) {
						// still infeasible in the reduced problem, vanishingly rare, keep whatever candidate we already had
						result = tempResult;
					}

					distance = lines[i].direction.cross(lines[i].point - result);
				}
			}
		}
	}

	ORCALine computeORCALine(
		const Vector2float& selfVelocity,
		const Vector2float& relativePosition,
		const Vector2float& neighborVelocity,
		f32 combinedRadius,
		f32 timeHorizon,
		f32 timeStep,
		f32 selfResponsibility)
	{
		Vector2float relativeVelocity = selfVelocity - neighborVelocity;
		f32 distSq = relativePosition.dot(relativePosition);
		f32 combinedRadiusSq = combinedRadius * combinedRadius;

		ORCALine line;
		Vector2float u;

		if (distSq > combinedRadiusSq) {
			// not currently overlapping, build the velocity-obstacle cone truncated at timeHorizon
			Vector2float w = relativeVelocity - relativePosition * (1.0f / timeHorizon);
			f32 wLengthSq = w.dot(w);
			f32 dotProduct1 = w.dot(relativePosition);

			if (dotProduct1 < 0.0f && dotProduct1 * dotProduct1 > combinedRadiusSq * wLengthSq) {
				// closest point on the obstacle is the truncation circle's cap
				f32 wLength = std::sqrt(wLengthSq);
				Vector2float unitW = w * (1.0f / wLength);
				line.direction = Vector2float{ unitW.y, -unitW.x };
				u = unitW * (combinedRadius / timeHorizon - wLength);
			}
			else {
				// closest point is on one of the cone's two side legs
				f32 leg = std::sqrt(distSq - combinedRadiusSq);
				if (relativePosition.cross(w) > 0.0f) {
					line.direction = Vector2float{
						relativePosition.x * leg - relativePosition.y * combinedRadius,
						relativePosition.x * combinedRadius + relativePosition.y * leg
					} * (1.0f / distSq);
				}
				else {
					line.direction = Vector2float{
						relativePosition.x * leg + relativePosition.y * combinedRadius,
						-relativePosition.x * combinedRadius + relativePosition.y * leg
					} * (-1.0f / distSq);
				}
				f32 dotProduct2 = relativeVelocity.dot(line.direction);
				u = line.direction * dotProduct2 - relativeVelocity;
			}
		}
		else {
			// already overlapping, push apart hard enough to separate within one simulation step
			f32 invTimeStep = 1.0f / std::max(timeStep, ORCA_EPSILON);
			Vector2float w = relativeVelocity - relativePosition * invTimeStep;
			f32 wLength = w.length();
			Vector2float unitW = wLength > ORCA_EPSILON ? w * (1.0f / wLength) : Vector2float{ 1.0f, 0.0f };
			line.direction = Vector2float{ unitW.y, -unitW.x };
			u = unitW * (combinedRadius * invTimeStep - wLength);
		}

		// reciprocal, this agent takes its own share of the responsibility, trusting the neighbor's own computation for the rest
		line.point = selfVelocity + u * selfResponsibility;
		return line;
	}

	Vector2float solveORCA(const std::vector<ORCALine>& lines, const Vector2float& preferredVelocity, f32 maxSpeed)
	{
		Vector2float result;
		size_t failedLine = linearProgram2(lines, maxSpeed, preferredVelocity, false, result);
		if (failedLine < lines.size()) {
			linearProgram3(lines, failedLine, maxSpeed, result);
		}
		return result;
	}
}
