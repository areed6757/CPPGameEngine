#pragma once
#include <Core/Common.h>
#include <GameECS.h>
#include <ECS/TickedSystem.h>
#include <Graphics/Renderer.h>
#include <Graphics/MeshRegistry.h>
#include <Graphics/TextureRegistry.h>

namespace Engine {
	struct RenderSystemDesc {
		BaseDesc base;
		GameECSWrapper& ecs;
		MeshRegistry& meshRegistry;
		TextureRegistry& textureRegistry;
		Renderer& renderer;
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
		std::bitset<64> m_entityMask;
	};
}