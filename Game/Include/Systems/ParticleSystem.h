#pragma once
#include <Core/Common.h>
#include <GameECS.h>
#include <Graphics/ComputeShader.h>
#include <ThirdParty/glad/glad.h>
#include <glm/glm.hpp>
#include <ECS/TickedSystem.h>
#include <Physics/Vector2double.h>

namespace Engine {
	struct ParticleSystemDesc {
		BaseDesc base;
		GameECSWrapper& ecs;
	};

	struct ParticleGPU {
		glm::vec2 position;
		glm::vec2 velocity;
		float age;
		float maxLifetime;
	};

	class ParticleSystem : public Base, public TickedSystem {
	public :
		explicit ParticleSystem(const ParticleSystemDesc& desc);
		~ParticleSystem();

		void Update(d64 dt) override;
		GLuint getBufferID() const noexcept { return m_ssbo; }

	private:
		void emit(const Vector2double& shipPos, const Vector2float& shipVel);

		GameECSWrapper& m_ecs;
		ComputeShader m_updateShader;
		GLuint m_ssbo{};
		i32 m_writeCursor{ 0 };

		static constexpr i32 MAX_PARTICLES = 100000;
		static constexpr i32 EMIT_PER_TICK = 4;
		static constexpr f32 BASE_SPEED = 2.0f;
		static constexpr f32 SPEED_SCALE = 0.3f;
		static constexpr f32 SPREAD = 1.5f;
		static constexpr f32 LIFETIME = 1.2f;

		void debugLogSample();
		i32 m_debugTickCounter{ 0 };
	};

}