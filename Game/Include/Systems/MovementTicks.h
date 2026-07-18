#pragma once
#include <Components/Movement.h>
#include <Components/Position.h>
#include <Core/Common.h>
#include <ECS/TickedSystem.h>
#include <GameDescs.h>
#include <GameECS.h>
#include <bitset>
#include <format>

namespace Engine {
	class MovementTicks final : public Base, public TickedSystem {
	public:
		explicit MovementTicks(const MovementTicksDesc& desc);
		~MovementTicks();

		void Update(d64 deltaTime) override;

	private:
		GameECSWrapper& m_ecs;
		std::bitset<64> m_entityMask;
		i32 m_tickCount = 0;
	};
}