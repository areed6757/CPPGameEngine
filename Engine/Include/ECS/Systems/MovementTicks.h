#pragma once
#include <bitset>
#include <Core/Common.h>
#include <ECS/ECSWrapper.h>
#include <ECS/Systems/TickedSystem.h>
#include <ECS/Components/Movement.h>
#include <ECS/Components/Transform.h>

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