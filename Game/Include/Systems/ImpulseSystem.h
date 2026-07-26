#pragma once
#include <Game.h>

namespace Engine {
	class CollisionSystem;

	struct ImpulseSystemDesc {
		BaseDesc base;
		GameECSWrapper& ecs;
		CollisionSystem& collisionSystem;
	};

	class ImpulseSystem : public Base, public TickedSystem {
	public:
		explicit ImpulseSystem(const ImpulseSystemDesc& desc);
		~ImpulseSystem();

		void Update(d64 dt) override;

	private:
		GameECSWrapper& m_ecs;
		CollisionSystem& m_collisionSystem;
	};

}