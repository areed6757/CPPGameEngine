#pragma once 
#include <GameECS.h>
#include <Core/Common.h>
#include <ECS/TickedSystem.h>

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

		std::bitset<64> getReadSignature() const noexcept { return m_reads; }
		std::bitset<64> getWriteSignature() const noexcept { return m_reads; }

	protected:
		std::bitset<64> m_reads{};
		std::bitset<64> m_writes{};

	private:
		GameECSWrapper& m_ecs;
		std::bitset<64> m_entityMask{};
	};
}