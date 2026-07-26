#pragma once
#include <Engine.h>
#include <GameECS.h>
#include <Systems/CollisionSystem.h>

namespace Engine {
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