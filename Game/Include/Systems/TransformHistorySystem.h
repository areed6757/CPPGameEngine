#pragma once
#include <Game.h>

namespace Engine {
	struct TransformHistorySystemDesc {
		BaseDesc base;
		GameECSWrapper& ecs;
	};

	// snapshots every Position's current pose into its prev pose once per fixed tick, must register before any other Position-writing system
	class TransformHistorySystem : public Base, public TickedSystem {
	public:
		explicit TransformHistorySystem(const TransformHistorySystemDesc& desc);
		~TransformHistorySystem();

		void Update(d64 dt) override;

	private:
		GameECSWrapper& m_ecs;
	};
}
