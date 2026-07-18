#include <Graphics/Mesh.h>

Engine::Mesh::Mesh(const MeshDesc& desc) : Base(desc.base)
{
}

void Engine::Mesh::Bind() const
{
}

GLsizei Engine::Mesh::indexCount() const
{
	return GLsizei();
}
