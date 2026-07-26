#pragma once 
#include <Engine.h>
#include <GameECS.h>

namespace Engine {
	struct ThrusterSystemDesc {
		BaseDesc base;
		GameECSWrapper& ecs;
	};

	class ThrusterSystem : public Base, public TickedSystem {
	public:
		explicit ThrusterSystem(const ThrusterSystemDesc& desc);
		~ThrusterSystem();

		void Update(d64 dt) override;

	private:
		GameECSWrapper& m_ecs;
		GameCommandBuffer m_cmdBuffer;
		std::bitset<64> m_entityMask{};
	};
}