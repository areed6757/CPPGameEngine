#pragma once
#include <glm/glm.hpp>

namespace Engine {
	struct ParticleGPU {
		glm::vec2 position;
		glm::vec2 velocity;
		float age;
		float maxLifetime;
	};
}