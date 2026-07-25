#include <Systems/ParticleSystem.h>
#include <random>
#include <cmath>

namespace Engine {
	ParticleSystem::ParticleSystem(const ParticleSystemDesc& desc) : Base(desc.base),
		m_ecs(desc.ecs),
		m_updateShader({ desc.base, "Shaders/particle_update.comp"} )
	{
		m_reads = m_ecs.makeSignature<Position, Movement, Thruster>();
		m_writes = m_ecs.makeSignature<>();

		glGenBuffers(1, &m_ssbo);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo);
		glBufferData(GL_SHADER_STORAGE_BUFFER, MAX_PARTICLES * sizeof(ParticleGPU), nullptr, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		EngineLogInfo("Particle system created({} max particles)", MAX_PARTICLES);
	}

	ParticleSystem::~ParticleSystem()
	{
		glDeleteBuffers(1, &m_ssbo);
	}

	void ParticleSystem::emit(const Vector2double& shipPos, const Vector2float& shipVel)
	{
		static std::mt19937 rng{ std::random_device{}() };
		static std::uniform_real_distribution<f32> spread(-1.0f, 1.0f);

		f32 speed = std::sqrt(shipVel.x * shipVel.x + shipVel.y * shipVel.y);
		Vector2float back{};
		if (speed > 1e-4f) { back = { -shipVel.x / speed, -shipVel.y / speed }; }
		Vector2float perp{ -back.y, back.x };

		std::vector<ParticleGPU> batch;
		batch.reserve(EMIT_PER_TICK);

		for (i32 n = 0; n < EMIT_PER_TICK; n++) {
			f32 outSpeed = BASE_SPEED + SPEED_SCALE * speed;
			f32 s = spread(rng) * SPREAD;

			ParticleGPU p;
			p.position = glm::vec2(static_cast<f32>(shipPos.x), static_cast<f32>(shipPos.y));
			p.velocity = glm::vec2(back.x * outSpeed + perp.x * s, back.y * outSpeed + perp.y * s);
			p.age = 0.0f;
			p.maxLifetime = LIFETIME;
			batch.push_back(p);
		}
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo);
		for (auto& p : batch) {
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, m_writeCursor * sizeof(ParticleGPU), sizeof(ParticleGPU), &p);
			m_writeCursor = (m_writeCursor + 1) % MAX_PARTICLES;
		}
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	void ParticleSystem::Update(d64 dt)
	{
		i32 c = m_ecs.sizeComponentPool<Thruster>();
		for (i32 i = 0; i < c; i++) {
			i32 entityIndex = m_ecs.entityAtDenseIndex<Thruster>(i);
			EntityID id = m_ecs.entityFromIndex(entityIndex);
			if ((m_ecs.getSignature(id) & m_reads) != m_reads) { continue; }

			auto& pos = m_ecs.getComponent<Position>(id);
			auto& mov = m_ecs.getComponent<Movement>(id);
			emit(pos.transform, mov.linearVelocity);
		}

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_ssbo);
		m_updateShader.Activate();
		GLuint dtLoc = glGetUniformLocation(m_updateShader.ID, "dt");
		glUniform1f(dtLoc, static_cast<f32>(dt));

		GLuint groups = (MAX_PARTICLES + 255) / 256;
		glDispatchCompute(groups, 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);


		// DEBUG LINES
		m_debugTickCounter++;
		if (m_debugTickCounter >= 60) {
			debugLogSample();
			m_debugTickCounter = 0;
		}
	}

	void ParticleSystem::debugLogSample()
	{
		ParticleGPU sample[4];
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo);
		glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(sample), sample);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		
		for (i32 i = 0; i < 4; i++) {
			EngineLogInfo("Particle[{}]: pos({}, {}) vel({}, {}) age {} / {}",
				i, sample[i].position.x, sample[i].position.y,
				sample[i].velocity.x, sample[i].velocity.y,
				sample[i].age, sample[i].maxLifetime);
		}
	}
}