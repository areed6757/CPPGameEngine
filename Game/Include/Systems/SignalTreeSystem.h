#pragma once
#include <Game.h>
#include <vector>
#include <mutex>

namespace Engine {
	struct SignalTreeSystemDesc {
		BaseDesc base;
		GameECSWrapper& ecs;
		AABBTree& signalTree;
	};

	// a pending magnitude change for one entity's SignalSignature, applied on the next SignalTreeSystem tick
	struct SignalDelta {
		EntityID target{};
		f32 delta{ 0.0f };
	};

	// keeps the signal tree's point proxies in sync with every Position+SignalSignature entity, and offers a
	// thread-safe queue for other systems (running on any thread) to push magnitude changes without racing
	// directly on SignalSignature, for stealth/heat/active-sensor sources to use later
	class SignalTreeSystem : public Base, public TickedSystem {
	public:
		explicit SignalTreeSystem(const SignalTreeSystemDesc& desc);
		~SignalTreeSystem();

		void Update(d64 dt) override;

		void queueSignalDelta(EntityID target, f32 delta);

	private:
		GameECSWrapper& m_ecs;
		AABBTree& m_signalTree;

		struct ProxyEntry { i32 proxyId = -1; i32 generation = -1; };
		std::vector<ProxyEntry> m_proxies;

		std::mutex m_pendingDeltasMutex;
		std::vector<SignalDelta> m_pendingDeltas;

		std::bitset<64> m_entityMask;
	};
}
