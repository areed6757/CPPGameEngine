#pragma once
#include <Core/Common.h>
#include <Graphics/Window.h>
#include <Graphics/Shader.h>
#include <Graphics/Texture.h>
#include <Graphics/Mesh.h>
#include <ThirdParty/glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <Graphics/Camera.h>

namespace Engine {
	struct RendererDesc {
		BaseDesc base;
		Window& window;
		ShaderDesc shaderDesc; // Pass-through
		Camera& camera;
	};

	class Renderer : public Base {
	public:
		explicit Renderer(const RendererDesc& desc);
		~Renderer();
		
		void beginFrame();
		void drawInstanced(const Mesh& mesh, const Texture* texture, GLsizei instanceCount);
		void endFrame();

	private:
		Window& m_window;
		Shader m_shader;
		Camera& m_camera;

		GLuint m_tex0uni;
		GLuint m_useTextureUni{};
		GLuint m_projectionUni{};
	};
}