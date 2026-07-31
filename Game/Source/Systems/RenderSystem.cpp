#include <Systems/RenderSystem.h>

Engine::RenderSystem::RenderSystem(const RenderSystemDesc& desc) : Base(desc.base),
	m_ecs(desc.ecs),
	m_meshReg(desc.meshRegistry),
	m_textureReg(desc.textureRegistry),
	m_renderer(desc.renderer),
	m_camera(desc.camera)
{
	m_entityMask = m_ecs.makeSignature<Position, Renderable>();
	m_reads = m_ecs.makeSignature<Position, Renderable>();
	m_writes = m_ecs.makeSignature<>();
	EngineLogInfo("Render system created.");
}

Engine::RenderSystem::~RenderSystem()
{
	EngineLogInfo("Render system destroyed.");
}

void Engine::RenderSystem::Update(d64 dt)
{
	for (auto& [key, matrices] : m_batches) { matrices.clear(); }

	i32 c = m_ecs.sizeComponentPool<Renderable>();
	for (i32 i = 0; i < c; i++) {
		i32 entityIndex = m_ecs.entityAtDenseIndex<Renderable>(i);
		EntityID id = m_ecs.entityFromIndex(entityIndex);
		if ((m_ecs.getSignature(id) & m_entityMask) != m_entityMask) continue;

		auto& renderable = m_ecs.getComponentAtDenseIndex<Renderable>(i);
		auto& position = m_ecs.getComponent<Position>(id);

		Vector2double relative = m_camera.toCameraRelative(position.transform);
		glm::vec3 pos(static_cast<f32>(relative.x), static_cast<f32>(relative.y), 0.0f);

		glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
		model = glm::rotate(model, position.rotation, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(renderable.scale, renderable.scale, 1.0f));

		m_batches[{ renderable.mesh, renderable.texture }].push_back(PartInstanceData{ model, 1.0f });
	}

	for (auto& [key, matrices] : m_batches) {
		if (matrices.empty()) { continue; }

		const Mesh& mesh = m_meshReg.get(key.first);
		const Texture* texturePtr = key.second.has_value() ? &m_textureReg.get(*key.second) : nullptr;

		mesh.uploadInstanceData(matrices.data(), matrices.size() * sizeof(PartInstanceData), static_cast<GLsizei>(matrices.size()));
		m_renderer.drawInstanced(mesh, texturePtr, static_cast<GLsizei>(matrices.size()));
	}
}
