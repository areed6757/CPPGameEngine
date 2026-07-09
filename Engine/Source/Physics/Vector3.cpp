#include <Physics/Vector3.h>

Engine::Vector3::Vector3() : m_x{0.0}, m_y{0.0}, m_z{0.0}
{

}

// This Vector3 is Z-locked for 2d implementation in a 3d space.
Engine::Vector3::Vector3(d64 x, d64 y) : m_x{x}, m_y{y}, m_z{ 0.0 }
{
}

Engine::Vector3::~Vector3()
{
}
