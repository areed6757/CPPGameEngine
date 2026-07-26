#include <Graphics/DebugLineRenderer.h>

namespace Engine {
	DebugLineRenderer::DebugLineRenderer(const DebugLineRendererDesc& desc) : Base(desc.base),
		m_shader(desc.shaderDesc)
	{
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

		m_VAO.Bind();
		m_VBO.BufferData(flatVerts.size() * sizeof(f32), flatVerts.data(), GL_DYNAMIC_DRAW);
		m_VAO.LinkAttrib(m_VBO, 0, 3, GL_FLOAT, 3 * sizeof(f32), (void*)0);

		glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(flatVerts.size() / 3));
	}
}