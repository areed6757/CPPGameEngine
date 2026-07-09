#include <Physics/Vector3.h>

Engine::Vector3::Vector3() : m_x{0.0}, m_y{0.0}, m_z{0.0}
{

}

Engine::Vector3::Vector3(d64 x, d64 y, d64 z) : m_x{x}, m_y{y}, m_z{z}
{
}

Engine::Vector3::~Vector3()
{
}
