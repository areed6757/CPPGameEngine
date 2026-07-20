#include <Systems/RenderSystem.h>
#include <glm/gtc/matrix_transform.hpp>

Engine::RenderSystem::RenderSystem(const RenderSystemDesc& desc) : Base(desc.base),
	m_ecs(desc.ecs),
	m_meshReg(desc.meshRegistry),
	m_textureReg(desc.textureRegistry),
	m_renderer(desc.renderer),
	m_camera(desc.camera)
{
	m_entityMask = m_ecs.makeSignature<Position, Renderable>();
	EngineLogInfo("Render system created.");
}

Engine::RenderSystem::~RenderSystem()
{
	EngineLogInfo("Render system destroyed.");
}

void Engine::RenderSystem::Update(d64 dt)
{
	i32 c = m_ecs.sizeComponentPool<Renderable>();
	for (i32 i = 0; i < c; i++) {
		i32 entityIndex = m_ecs.entityAtDenseIndex<Renderable>(i);
		EntityID id = m_ecs.entityFromIndex(entityIndex);
		if ((m_ecs.getSignature(id) & m_entityMask) != m_entityMask) continue;

		auto& renderable = m_ecs.getComponentAtDenseIndex<Renderable>(i);
		auto& position = m_ecs.getComponent<Position>(id); // Needed after implmenting model matrix + camera

		Vector2double relative = m_camera.toCameraRelative(position.transform);
		glm::vec3 pos(static_cast<f32>(relative.x), static_cast<f32>(relative.y), 0.0f);

		glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
		model = glm::rotate(model, position.rotation, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(renderable.scale, renderable.scale, 1.0f));

		const Mesh& mesh = m_meshReg.get(renderable.mesh);
		const Texture* texturePtr = renderable.texture.has_value() ? &m_textureReg.get(*renderable.texture) : nullptr;

		m_renderer.draw(mesh, texturePtr, model);

	}
}
