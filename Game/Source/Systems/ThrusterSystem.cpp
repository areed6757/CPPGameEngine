#include <Systems/ThrusterSystem.h>
#include <Components/Thruster.h>
#include <Components/Movement.h>
#include <Components/Position.h>
#include <cmath>


namespace Engine {
	ThrusterSystem::ThrusterSystem(const ThrusterSystem& desc) : Base(desc.base),
		m_ecs(desc.ecs)
	{
		m_entityMask = m_ecs.makeSignature<Position, Movement, Thruster>();
	}

	ThrusterSystem::~ThrusterSystem()
	{
	}

	void ThrusterSystem::Update(d64 dt)
	{

	}

}