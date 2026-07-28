#pragma once
#include <Game.h>

namespace Engine {
	struct MountFollowSystemDesc {
		BaseDesc base;
		GameECSWrapper& ecs;
	};

	class MountFollowSystem : public Base, public TickedSystem {
	public:
		explicit MountFollowSystem(const MountFollowSystemDesc& desc);
		~MountFollowSystem();

		void Update(d64 dt) override;

	private:
		GameECSWrapper& m_ecs;
		std::bitset<64> m_entityMask;
	};
}