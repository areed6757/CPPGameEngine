#include <Graphics/Mesh.h>

Engine::Mesh::Mesh(const MeshDesc& desc) : Base(desc.base),
	m_VAO(),
	m_VBO(desc.vertices, desc.verticesSize),
	m_EBO(desc.indices, desc.indicesSize),
	m_indexCount(desc.indexCount)
{
	m_VAO.Bind();
	m_EBO.Bind();
	
	m_VAO.LinkAttrib(m_VBO, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
	m_VAO.LinkAttrib(m_VBO, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	m_VAO.LinkAttrib(m_VBO, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));

	m_VAO.Unbind();
	m_VBO.Unbind();
	m_EBO.Unbind();
}

void Engine::Mesh::Bind() const
{
	m_VAO.Bind();
}

GLsizei Engine::Mesh::indexCount() const
{
	return m_indexCount;
}
