#include <Systems/PartRenderSystem.h>

namespace Engine {
	PartRenderSystem::PartRenderSystem(const PartRenderSystemDesc& desc) : Base(desc.base),
		m_ecs(desc.ecs),
		m_renderer(desc.renderer),
		m_camera(desc.camera),
		m_window(desc.window),
		m_meshReg(desc.meshReg),
		m_partReg(desc.partReg),
		m_textureReg(desc.textureReg),
		m_scheduler(desc.scheduler)
	{
		m_entityMask = m_ecs.makeSignature<Position, ShipVisual>();
		m_reads = m_ecs.makeSignature<Position, ShipVisual>();
		m_writes = m_ecs.makeSignature<>();
	}

	PartRenderSystem::~PartRenderSystem()
	{

	}

	void PartRenderSystem::Update(d64 dt)
	{
		for (auto& [key, matrices] : m_batches) { matrices.clear(); }
		for (auto& [key, matrices] : m_iconBatches) { matrices.clear(); }

		AABB viewport = m_camera.getViewportBounds(m_window.getWidth(), m_window.getHeight());

		f32 worldUnitsPerPixel = m_camera.getWorldUnitsPerPixel(m_window.getHeight());
		f32 alpha = m_scheduler.getInterpolationAlpha();

		i32 c = m_ecs.sizeComponentPool<ShipVisual>();
		for (i32 i = 0; i < c; i++) {
			i32 entityIndex = m_ecs.entityAtDenseIndex<ShipVisual>(i);
			EntityID id = m_ecs.entityFromIndex(entityIndex);
			if ((m_ecs.getSignature(id) & m_entityMask) != m_entityMask) { continue; }

			auto& shipPos = m_ecs.getComponent<Position>(id);
			auto& visual = m_ecs.getComponentAtDenseIndex<ShipVisual>(i);
			InterpolatedPose pose = interpolatePosition(shipPos, alpha);

			i32 tier = tierForPartCount(static_cast<i32>(visual.parts.size()));
			const ShipIconTier& iconTier = g_shipIconTiers[tier];

			f32 shipRadius = 0.0f;
			if (m_ecs.hasComponent<Physics>(id)) {
				auto& shipPhysics = m_ecs.getComponent<Physics>(id);
				shipRadius = shipPhysics.radius;
				f32 cullRadius = std::max(shipRadius, iconTier.iconPixelSize * worldUnitsPerPixel);
				AABB shipBounds{
					Vector2double{ pose.transform.x - cullRadius, pose.transform.y - cullRadius },
					Vector2double{ pose.transform.x + cullRadius, pose.transform.y + cullRadius }
				};
				if (!viewport.overlaps(shipBounds)) { continue; }
			}

			f32 shipPixelDiameter = (shipRadius * 2.0f) / worldUnitsPerPixel;
			if (shipPixelDiameter < iconTier.swapThreshold) {
				f32 iconWorldScale = iconTier.iconPixelSize * worldUnitsPerPixel;

				Vector2double relative = m_camera.toCameraRelative(pose.transform);
				glm::mat4 model = glm::translate(glm::mat4(1.0f),
					glm::vec3(static_cast<f32>(relative.x), static_cast<f32>(relative.y), 0.0f));
				model = glm::rotate(model, pose.rotation + ICON_ART_ROTATION_OFFSET, glm::vec3(0.0f, 0.0f, 1.0f));
				model = glm::scale(model, glm::vec3(iconWorldScale, iconWorldScale, 1.0f));

				m_iconBatches[iconTier.texture].push_back(PartInstanceData{ model, 1.0f });
				continue;
			}

			f32 shipHealthFraction = 1.0f;
			if (m_ecs.hasComponent<Stability>(id)) {
				auto& stability = m_ecs.getComponent<Stability>(id);
				shipHealthFraction = (stability.max > 0.0f) ? (stability.current / stability.max) : 1.0f;
			}

			f32 rot = pose.rotation;
			f32 cosR = std::cos(rot), sinR = std::sin(rot);
			for (auto& part : visual.parts) {
				Vector2float anchorLocal = part.localOffset + part.anchorOffset;
				Vector2double worldOffset{
					anchorLocal.x * cosR - anchorLocal.y * sinR,
					anchorLocal.x * sinR + anchorLocal.y * cosR
				};
				Vector2double worldPos = pose.transform + worldOffset;
				Vector2double relative = m_camera.toCameraRelative(worldPos);

				glm::vec3 pos(static_cast<f32>(relative.x), static_cast<f32>(relative.y), 0.0f);
				glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
				model = glm::rotate(model, rot, glm::vec3(0.0f, 0.0f, 1.0f));
				model = glm::translate(model, glm::vec3(-part.anchorOffset.x, -part.anchorOffset.y, 0.0f));
				f32 partScale = std::max(part.sizeX, part.sizeY) * static_cast<f32>(GRID_CELL_SIZE_KM);
				model = glm::scale(model, glm::vec3(partScale, partScale, 1.0f));

				f32 healthFraction = shipHealthFraction;
				if (part.category == PartCategory::Hardpoint && m_ecs.hasComponent<Health>(part.linkedEntity)) {
					auto& health = m_ecs.getComponent<Health>(part.linkedEntity);
					healthFraction = (health.max > 0.0f) ? (health.current / health.max) : 1.0f;
				}

				m_batches[{ part.category, part.variant }].push_back(PartInstanceData{ model, healthFraction });
			}
		}

		for (auto& [key, matrices] : m_batches) {
			if (matrices.empty()) { continue; }
			const Mesh& mesh = m_meshReg.get(MeshID::Quad);
			mesh.uploadInstanceData(matrices.data(), matrices.size() * sizeof(PartInstanceData), static_cast<GLsizei>(matrices.size()));
			m_renderer.drawInstanced(mesh, nullptr, static_cast<GLsizei>(matrices.size()));
		}

		for (auto& [texture, matrices] : m_iconBatches) {
			if (matrices.empty()) { continue; }
			const Mesh& iconMesh = m_meshReg.get(MeshID::Quad);
			const Texture& iconTexture = m_textureReg.get(texture);
			iconMesh.uploadInstanceData(matrices.data(), matrices.size() * sizeof(PartInstanceData), static_cast<GLsizei>(matrices.size()));
			m_renderer.drawInstanced(iconMesh, &iconTexture, static_cast<GLsizei>(matrices.size()));
		}
	}
}