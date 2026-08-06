#pragma once
#include <Game.h>
#include <vector>

namespace Engine {
	struct SignalTreeSystemDesc {
		BaseDesc base;
		GameECSWrapper& ecs;
		AABBTree& signalTree;
	};

	// keeps the signal tree's point proxies in sync with every Position+SignalSignature entity, nothing queries this tree yet
	class SignalTreeSystem : public Base, public TickedSystem {
	public:
		explicit SignalTreeSystem(const SignalTreeSystemDesc& desc);
		~SignalTreeSystem();

		void Update(d64 dt) override;

	private:
		GameECSWrapper& m_ecs;
		AABBTree& m_signalTree;

		struct ProxyEntry { i32 proxyId = -1; i32 generation = -1; };
		std::vector<ProxyEntry> m_proxies;

		std::bitset<64> m_entityMask;
	};
}
