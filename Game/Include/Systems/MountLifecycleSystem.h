#pragma once
#include <Game.h>

namespace Engine {
	struct MountLifecycleSystemDesc {
		BaseDesc base;
		GameECSWrapper& ecs;
	};

	class MountLifecycleSystem : public Base, public TickedSystem {
	public:
		explicit MountLifecycleSystem(const MountLifecycleSystemDesc& desc);
		~MountLifecycleSystem();

		void Update(d64 dt) override;

		GameCommandBuffer& getCommandBuffer() noexcept { return m_cmdBuffer; }

	private:
		GameECSWrapper& m_ecs;
		GameCommandBuffer m_cmdBuffer;
		std::bitset<64> m_entityMask{};
	};
}