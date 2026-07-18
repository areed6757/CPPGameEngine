#include <Graphics/MeshRegistry.h>

Engine::MeshRegistry::MeshRegistry(const MeshRegistryDesc& desc) :
	Base(desc.base),
	m_meshes{
		// Instantiation must be in the order declared in MeshID
		Mesh{desc.base, quadVertices, sizeof(quadVertices), quadIndices, sizeof(quadIndices), 6},
		Mesh{desc.base, triangleVertices, sizeof(triangleVertices), triangleIndices, sizeof(triangleIndices), 3},
	}
{

}

Engine::MeshRegistry::~MeshRegistry()
{
}

const Mesh& Engine::MeshRegistry::get(MeshID id) const
{
	return m_meshes[0];
}
