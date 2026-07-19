#include <Systems/RenderSystem.h>

Engine::RenderSystem::RenderSystem(const RenderSystemDesc& desc) : Base(desc.base),
	m_ecs(desc.ecs),
	m_meshReg(desc.meshRegistry),
	m_textureReg(desc.textureRegistry),
	m_renderer(desc.renderer)
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
	m_renderer.beginFrame();

	i32 c = m_ecs.sizeComponentPool<Renderable>();
	for (i32 i = 0; i < c; i++) {
		i32 entityIndex = m_ecs.entityAtDenseIndex<Renderable>(i);
		EntityID id = m_ecs.entityFromIndex(entityIndex);
		if ((m_ecs.getSignature(id) & m_entityMask) != m_entityMask) continue;

		auto& renderable = m_ecs.getComponentAtDenseIndex<Renderable>(i);
		// auto& tform = m_ecs.getComponent<Position>(id); // Needed after implmenting model matrix + camera

		const Mesh& mesh = m_meshReg.get(renderable.mesh);
		const Texture* texturePtr = renderable.texture.has_value() ? &m_textureReg.get(*renderable.texture) : nullptr;

		m_renderer.draw(mesh, texturePtr);

	}

	m_renderer.endFrame();

}
