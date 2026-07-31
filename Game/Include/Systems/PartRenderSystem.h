#pragma once
#include <Game.h>
#include <map>

namespace Engine {
	class PartRegistry;

	struct PartRenderSystemDesc {
		BaseDesc base;
		GameECSWrapper& ecs;
		Renderer& renderer;
		Camera& camera;
		MeshRegistry& meshReg;
		PartRegistry& partReg;
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
		MeshRegistry& m_meshReg;
		PartRegistry& m_partReg;
		std::bitset<64> m_entityMask;

		std::map<std::pair<PartCategory, PartVariantID>, std::vector<PartInstanceData>> m_batches;
	};
}