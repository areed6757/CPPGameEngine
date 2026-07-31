#pragma once
#include <Game.h>
#include <map>

namespace Engine {
	class MeshRegistry;
	class TextureRegistry;

	struct RenderSystemDesc {
		BaseDesc base;
		GameECSWrapper& ecs;
		MeshRegistry& meshRegistry;
		TextureRegistry& textureRegistry;
		Renderer& renderer;
		Camera& camera;
		Window& window;
	};

	class RenderSystem : public Base, public TickedSystem {
	public :
		explicit RenderSystem(const RenderSystemDesc& desc);
		~RenderSystem();
		void Update(d64 dt) override;

	private:
		GameECSWrapper& m_ecs;
		MeshRegistry& m_meshReg;
		TextureRegistry& m_textureReg;
		Renderer& m_renderer;
		Camera& m_camera;
		Window& m_window;
		std::bitset<64> m_entityMask;
		std::map<std::pair<MeshID, std::optional<TextureID>>, std::vector<PartInstanceData>> m_batches;
	};
}