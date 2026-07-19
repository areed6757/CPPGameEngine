#pragma once
#include <Core/Common.h>
#include <GameECS.h>
#include <ECS/TickedSystem.h>
#include <Graphics/Renderer.h>
#include <Graphics/MeshRegistry.h>
#include <Graphics/TextureRegistry.h>
#include <Graphics/Camera.h>

namespace Engine {
	struct RenderSystemDesc {
		BaseDesc base;
		GameECSWrapper& ecs;
		MeshRegistry& meshRegistry;
		TextureRegistry& textureRegistry;
		Renderer& renderer;
		Camera& camera;
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
		std::bitset<64> m_entityMask;
	};
}