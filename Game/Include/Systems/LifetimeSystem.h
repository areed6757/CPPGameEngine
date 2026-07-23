#pragma once
#include <GameECS.h>
#include <Core/Common.h>
#include <ECS/TickedSystem.h>

namespace Engine {
	struct LifetimeSystemDesc {
		BaseDesc base;
		GameECSWrapper& ecs;
	};

	class LifetimeSystem : public Base, public TickedSystem {
	public:
		explicit LifetimeSystem(const LifetimeSystemDesc& desc);
		~LifetimeSystem();

		void Update(d64 dt) override;

		std::bitset<64> getReadSignature() const noexcept { return m_reads; }
		std::bitset<64> getWriteSignature() const noexcept { return m_writes; }

		GameCommandBuffer& getCommandBuffer() noexcept { return m_cmdBuffer; }

	protected:
		std::bitset<64> m_reads{};
		std::bitset<64> m_writes{};

	private:
		GameECSWrapper& m_ecs;
		GameCommandBuffer m_cmdBuffer;
		std::bitset<64> m_entityMask{};
	};
}