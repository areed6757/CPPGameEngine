#include <Test/ThreadingStressTest.h>
#include <Components/Position.h>
#include <Components/Movement.h>
#include <cmath>

namespace Engine {
	ThreadingStressTest::ThreadingStressTest(const ThreadingStressTestDesc& desc) : Base(desc.base), m_ecs(desc.ecs) {}
	ThreadingStressTest::~ThreadingStressTest() {}

	void ThreadingStressTest::spawnGrid(i32 count, d64 spacing)
	{
		auto start = std::chrono::high_resolution_clock::now();

		i32 side = static_cast<i32>(std::ceil(std::sqrt(static_cast<d64>(count))));
		d64 half = (side - 1) * spacing / 2.0;

		i32 spawned = 0;
		for (i32 y = 0; y < side && spawned < count; ++y) {
			for (i32 x = 0; x < side && spawned < count; ++x) {
				Vector2double pos{ x * spacing - half, y * spacing - half };

				// Deterministic per-entity variation (no RNG dependency) --
				// gives MovementTicks genuinely divergent per-entity math
				// instead of every entity doing the exact same arithmetic.
				f32 angle = static_cast<f32>((x * 37 + y * 71) % 360) * 0.0174533f; // degrees to radians
				Vector2float accel{ std::cos(angle) * 0.5f, std::sin(angle) * 0.5f };

				EntityID id = m_ecs.createEntity();
				m_ecs.addComponent(id, Position{ .transform = pos, .rotation = angle });
				m_ecs.addComponent(id, Movement{
					.linearVelocity = Vector2float{},
					.angularVelocity = 0.0f,
					.linearAcceleration = accel,
					.angularAcceleration = 0.0f
					});
				spawned++;
			}
		}

		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::high_resolution_clock::now() - start).count();
		EngineLogInfo("ThreadingStressTest: spawned {} entities in {}ms", spawned, ms);
	}
}