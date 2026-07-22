#pragma once 
#include <GameECS.h>
#include <Core/Common.h>

namespace Engine {
	struct ThrusterSystemDesc {
		BaseDesc base;
		ECSWrapper& ecs;
	};

	class ThrusterSystem : public base, public TickedSystem {
	public:
		explicit ThrusterSystem(const ThrusterSystem& desc);
		~ThrusterSystem();

		void Update(d64 dt) override;

	private:
		ECSWrapper& m_ecs;
		std::bitset<64> m_entityMask{};
	};
}