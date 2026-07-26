#pragma once
#include <Core/Common.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Graphics/Shader.h>
#include <Graphics/Texture.h>
#include <Graphics/Mesh.h>
#include <Graphics/VAO.h>
#include <Graphics/VBO.h>
#include <ThirdParty/glad/glad.h>
#include <GLFW/glfw3.h>

namespace Engine {
	struct DebugLineRendererDesc {
		BaseDesc base;
		ShaderDesc shaderDesc;
	};

	class DebugLineRenderer : public Base {
	public:
		explicit DebugLineRenderer(const DebugLineRendererDesc& desc);
		~DebugLineRenderer();

		void draw(const std::vector<f32>& flatVerts, const glm::mat4& projection, const glm::vec3& color);

	private:
		Shader m_shader;
		VAO m_VAO;
		VBO m_VBO;
		GLint m_projectionUni{};
		GLint m_modelUni{};
		GLint m_colorUni{};
	};

}