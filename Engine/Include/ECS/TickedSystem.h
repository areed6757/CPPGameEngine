#pragma once
#include <Core/Common.h>

// Base class for all systems that are updated per-tick.
// DeltaTime is used to determine actual changes made per tick-call to Update()
namespace Engine {
	class TickedSystem {
	public:
		virtual void Update(d64 deltaTime) = 0;
		virtual ~TickedSystem() = default;

		std::bitset<64> getReadSignature() const noexcept { return m_reads; }
		std::bitset<64> getReadSignature() const noexcept { return m_writes; }

	protected:
		std::bitset<64> m_reads{};
		std::bitset<64> m_writes{};
	};
}