#include <Systems/PartRenderSystem.h>

namespace Engine {
	PartRenderSystem::PartRenderSystem(const PartRenderSystemDesc& desc) : Base(desc.base),
		m_ecs(desc.ecs),
		m_renderer(desc.renderer),
		m_camera(desc.camera),
		m_meshReg(desc.meshReg),
		m_partReg(desc.partReg)
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

		i32 c = m_ecs.sizeComponentPool<ShipVisual>();
		for (i32 i = 0; i < c; i++) {
			i32 entityIndex = m_ecs.entityAtDenseIndex<ShipVisual>(i);
			EntityID id = m_ecs.entityFromIndex(entityIndex);
			if ((m_ecs.getSignature(id) & m_entityMask) != m_entityMask) { continue; }

			auto& shipPos = m_ecs.getComponent<Position>(id);
			auto& visual = m_ecs.getComponentAtDenseIndex<ShipVisual>(i);

			f32 shipHealthFraction = 1.0f;
			if (m_ecs.hasComponent<Stability>(id)) {
				auto& stability = m_ecs.getComponent<Stability>(id);
				shipHealthFraction = (stability.max > 0.0f) ? (stability.current / stability.max) : 1.0f;
			}

			f32 rot = shipPos.rotation;
			f32 cosR = std::cos(rot), sinR = std::sin(rot);

			for (auto& part : visual.parts) {
				Vector2double worldOffset{
					part.localOffset.x * cosR - part.localOffset.y * sinR,
					part.localOffset.x * sinR + part.localOffset.y * cosR
				};
				Vector2double worldPos = shipPos.transform + worldOffset;
				Vector2double relative = m_camera.toCameraRelative(worldPos);

				glm::vec3 pos(static_cast<f32>(relative.x), static_cast<f32>(relative.y), 0.0f);
				glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
				model = glm::rotate(model, rot, glm::vec3(0.0f, 0.0f, 1.0f));
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
			const Mesh& mesh = m_meshReg.get(MeshID::Quad); // TODO: Remove standard quad and add per-category geometry/shading
			mesh.uploadInstanceData(matrices.data(), matrices.size() * sizeof(PartInstanceData), static_cast<GLsizei>(matrices.size()));
			m_renderer.drawInstanced(mesh, nullptr, static_cast<GLsizei>(matrices.size()));
		}
	}
}