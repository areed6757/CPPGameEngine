#pragma once
#include <bitset>
#include <format>
#include <Core/Common.h>
#include <ECSWrapper.h>
#include <ECS/TickedSystem.h>
#include <Components/Movement.h>
#include <Components/Position.h>

namespace Engine {
	class MovementTicks final : public Base, public TickedSystem {
	public:
		explicit MovementTicks(const MovementTicksDesc& desc);
		~MovementTicks();

		void Update(d64 deltaTime) override;

	private:
		ECSWrapper& m_ecs;
		std::bitset<64> m_entityMask;
		i32 m_tickCount = 0;
	};
}