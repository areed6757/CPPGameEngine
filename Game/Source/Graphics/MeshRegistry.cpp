#include <Graphics/MeshRegistry.h>

Engine::MeshRegistry::MeshRegistry(const MeshRegistryDesc& desc) :
	Base(desc.base),
	m_meshes{
		// Instantiation must be in the order declared in MeshID
		Mesh{ MeshDesc{desc.base, quadVertices, sizeof(quadVertices), quadIndices, sizeof(quadIndices), 6} },
		Mesh{ MeshDesc{desc.base, triangleVertices, sizeof(triangleVertices), triangleIndices, sizeof(triangleIndices), 3} },
	}
{

}

Engine::MeshRegistry::~MeshRegistry()
{
}

const Engine::Mesh& Engine::MeshRegistry::get(MeshID id) const
{
	return m_meshes[static_cast<size_t>(id)];
}
