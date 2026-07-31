#include <Graphics/Mesh.h>
#include <glm/gtc/matrix_transform.hpp>

namespace Engine {
	Mesh::Mesh(const MeshDesc& desc) : Base(desc.base),
		m_VAO(),
		m_VBO(desc.vertices, desc.verticesSize),
		m_EBO(desc.indices, desc.indicesSize),
		m_indexCount(desc.indexCount),
		m_instanceVBO()
	{
		m_VAO.Bind();
		m_EBO.Bind();

		m_VAO.LinkAttrib(m_VBO, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
		m_VAO.LinkAttrib(m_VBO, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		m_VAO.LinkAttrib(m_VBO, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));

		for (GLuint i = 0; i < 4; i++) {
			m_VAO.LinkAttrib(m_instanceVBO, 3 + i, 4, GL_FLOAT, sizeof(PartInstanceData), (void*)(i * sizeof(glm::vec4)), 1);
		}
		m_VAO.LinkAttrib(m_instanceVBO, 7, 1, GL_FLOAT, sizeof(PartInstanceData), (void*)offsetof(PartInstanceData, healthFraction), 1);

		m_VAO.Unbind();
		m_VBO.Unbind();
		m_EBO.Unbind();
	}

	void Mesh::Bind() const
	{
		m_VAO.Bind();
	}

	GLsizei Mesh::indexCount() const
	{
		return m_indexCount;
	}

	void Mesh::uploadInstanceData(const void* data, GLsizeiptr dataSize, GLsizei instanceCount) const
	{
		m_instanceVBO.BufferData(dataSize, data, GL_DYNAMIC_DRAW);
		m_maxInstancesUploaded = instanceCount;
	}

	void Mesh::drawInstanced(GLsizei instanceCount) const
	{
		m_VAO.Bind();
		glDrawElementsInstanced(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0, instanceCount);
	}
}