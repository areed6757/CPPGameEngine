#include <Systems/RenderSystem.h>
#include <Ships/IconLOD.h>
#include <algorithm>

Engine::RenderSystem::RenderSystem(const RenderSystemDesc& desc) : Base(desc.base),
	m_ecs(desc.ecs),
	m_meshReg(desc.meshRegistry),
	m_textureReg(desc.textureRegistry),
	m_renderer(desc.renderer),
	m_camera(desc.camera),
	m_window(desc.window),
	m_scheduler(desc.scheduler)
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

	AABB viewport = m_camera.getViewportBounds(m_window.getWidth(), m_window.getHeight());
	f32 worldUnitsPerPixel = m_camera.getWorldUnitsPerPixel(m_window.getHeight());
	f32 alpha = m_scheduler.getInterpolationAlpha();

	i32 c = m_ecs.sizeComponentPool<Renderable>();
	for (i32 i = 0; i < c; i++) {
		i32 entityIndex = m_ecs.entityAtDenseIndex<Renderable>(i);
		EntityID id = m_ecs.entityFromIndex(entityIndex);
		if ((m_ecs.getSignature(id) & m_entityMask) != m_entityMask) continue;

		if (m_ecs.hasComponent<Mount>(id)) {
			auto& mount = m_ecs.getComponent<Mount>(id);
			if (m_ecs.isValidEntity(mount.owner) && m_ecs.hasComponent<ShipVisual>(mount.owner) && m_ecs.hasComponent<Physics>(mount.owner)) {
				auto& ownerVisual = m_ecs.getComponent<ShipVisual>(mount.owner);
				auto& ownerPhysics = m_ecs.getComponent<Physics>(mount.owner);
				if (shouldRenderAsIcon(static_cast<i32>(ownerVisual.parts.size()), ownerPhysics.radius, worldUnitsPerPixel)) { continue; }
			}
		}

		auto& renderable = m_ecs.getComponentAtDenseIndex<Renderable>(i);
		auto& position = m_ecs.getComponent<Position>(id);
		InterpolatedPose pose = interpolatePosition(position, alpha);

		f32 cullScale = renderable.scale;
		if (renderable.iconTexture.has_value()) {
			cullScale = std::max(cullScale, renderable.iconMinPixelSize * worldUnitsPerPixel);
		}

		AABB entityBounds{
			Vector2double{ pose.transform.x - cullScale, pose.transform.y - cullScale },
			Vector2double{ pose.transform.x + cullScale, pose.transform.y + cullScale }
		};
		if (!viewport.overlaps(entityBounds)) { continue; }

		f32 scale = renderable.scale;
		std::optional<TextureID> texture = renderable.texture;
		f32 rotation = pose.rotation + renderable.rotationOffset;

		if (renderable.iconTexture.has_value()) {
			f32 pixelDiameter = renderable.scale / worldUnitsPerPixel;
			if (pixelDiameter < renderable.iconMinPixelSize) {
				scale = renderable.iconMinPixelSize * worldUnitsPerPixel;
				texture = renderable.iconTexture;
				rotation += ICON_ART_ROTATION_OFFSET;
			}
		}

		Vector2double relative = m_camera.toCameraRelative(pose.transform);
		glm::vec3 pos(static_cast<f32>(relative.x), static_cast<f32>(relative.y), 0.0f);

		glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
		model = glm::rotate(model, rotation, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(scale, scale, 1.0f));

		m_batches[{ renderable.mesh, texture }].push_back(PartInstanceData{ model, 1.0f });
	}

	for (auto& [key, matrices] : m_batches) {
		if (matrices.empty()) { continue; }

		const Mesh& mesh = m_meshReg.get(key.first);
		const Texture* texturePtr = key.second.has_value() ? &m_textureReg.get(*key.second) : nullptr;

		mesh.uploadInstanceData(matrices.data(), matrices.size() * sizeof(PartInstanceData), static_cast<GLsizei>(matrices.size()));
		m_renderer.drawInstanced(mesh, texturePtr, static_cast<GLsizei>(matrices.size()));
	}
}
