#pragma once
#include <Game.h>
#include <vector>

namespace Engine {
	struct DetectionSystemDesc {
		BaseDesc base;
		GameECSWrapper& ecs;
		AABBTree& signalTree;
		ThreadPool& threadPool;
	};

	// resolves AIController::target/targetResolution from the signal tree instead of ground-truth nearest-enemy,
	// one query per AI ship, fanned out across m_threadPool the same way CollisionSystem::broadPhase does its own
	// direct fork-join (bypassing JobController's chunking, since the tree query itself is the external resource
	// that must be safe to read from many threads at once, guaranteed by the SignalTreeSystem ordering constraint)
	class DetectionSystem : public Base, public TickedSystem {
	public:
		explicit DetectionSystem(const DetectionSystemDesc& desc);
		~DetectionSystem();

		void Update(d64 dt) override;

	private:
		GameECSWrapper& m_ecs;
		AABBTree& m_signalTree;
		ThreadPool& m_threadPool;

		std::bitset<64> m_entityMask;
	};
}
