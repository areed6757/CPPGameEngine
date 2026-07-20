#pragma once
#include <Core/Common.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Graphics/Shader.h>
#include <Graphics/Texture.h>
#include <Graphics/Mesh.h>
#include <ThirdParty/glad/glad.h>
#include <GLFW/glfw3.h>

namespace Engine {
	class DebugLineRenderer : public Base {
	public:
		explicit DebugLineRenderer(const DebugLineRendererDesc& desc);
		~DebugLineRenderer();

		void draw(const std::vector<f32>& flatVerts, const glm::mat4& projection, const glm::vec3& color);
	
	private:
		Shader m_shader;
		GLuint m_VAO{};
		GLuint m_VBO{};
		GLint m_projectionUni{};
		GLint m_modelUni{};
		GLint m_colorUni{};
		
	
	};

}