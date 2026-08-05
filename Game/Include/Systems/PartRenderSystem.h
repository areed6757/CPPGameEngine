#pragma once
#include <Game.h>
#include <Ships/IconLOD.h>
#include <map>

namespace Engine {
	class PartRegistry;
	class TextureRegistry;

	struct PartRenderSystemDesc {
		BaseDesc base;
		GameECSWrapper& ecs;
		Renderer& renderer;
		Camera& camera;
		Window& window;
		MeshRegistry& meshReg;
		PartRegistry& partReg;
		TextureRegistry& textureReg;
	};

	// Complex draw class using ship part baked indices, this will eventually perform automatic visual generation for
	// ships based on parts placed
	class PartRenderSystem : public Base, public TickedSystem {
	public:
		explicit PartRenderSystem(const PartRenderSystemDesc& desc);
		~PartRenderSystem();

		void Update(d64 dt) override;

	private:
		GameECSWrapper& m_ecs;
		Renderer& m_renderer;
		Camera& m_camera;
		Window& m_window;
		MeshRegistry& m_meshReg;
		PartRegistry& m_partReg;
		TextureRegistry& m_textureReg;
		std::bitset<64> m_entityMask;

		std::map<std::pair<PartCategory, PartVariantID>, std::vector<PartInstanceData>> m_batches;
		std::map<TextureID, std::vector<PartInstanceData>> m_iconBatches;
	};
}