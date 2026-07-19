#pragma once
#include <Core/Common.h>
#include <Graphics/Window.h>
#include <Graphics/Shader.h>
#include <Graphics/Texture.h>
#include <Graphics/Mesh.h>
#include <ThirdParty/glad/glad.h>
#include <GLFW/glfw3.h>

namespace Engine {
	class Renderer : public Base {
	public:
		explicit Renderer(const RendererDesc& desc);
		~Renderer();
		
		void beginFrame();
		void draw(const Mesh& mesh, const Texture* texture);
		void endFrame();

	private:
		Window& m_window;

		Shader m_shader;

		GLuint m_uniID;
		GLuint m_tex0uni;
		GLuint m_useTextureUni{};
	};
}