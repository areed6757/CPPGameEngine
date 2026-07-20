#include <Graphics/DebugLineRenderer.h>

namespace Engine {
	Engine::DebugLineRenderer::DebugLineRenderer(const DebugLineRendererDesc& desc) : Base(desc.base),
		m_shader(desc.shaderDesc)
	{
		glGenVertexArrays(1, &m_VAO);
		glGenBuffers(1, &m_VBO);
		m_projectionUni = glGetUniformLocation(m_shader.ID, "projection");
		m_modelUni = glGetUniformLocation(m_shader.ID, "model");
		m_colorUni = glGetUniformLocation(m_shader.ID, "linecolor");
	}

	DebugLineRenderer::~DebugLineRenderer()
	{
	}

	void DebugLineRenderer::draw(const std::vector<f32>& flatVerts, const glm::mat4& projection, const glm::vec3& color)
	{
		m_shader.Activate();
		glm::mat4 identity(1.0f);
		glUniformMatrix4fv(m_projectionUni, 1, GL_FALSE, &projection[0][0]);
		glUniformMatrix4fv(m_modelUni, 1, GL_FALSE, &identity[0][0]);
		glUniform3fv(m_colorUni, 1, &color[0]);

		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, flatVerts.size() * sizeof(f32), flatVerts.data(), GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), (void*)0);
		glEnableVertexAttribArray(0);

		glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(flatVerts.size() / 3));
	}
}