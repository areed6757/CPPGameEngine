#pragma once
#include <Core/Common.h>
#include <Graphics/Window.h>
#include <Graphics/Shader.h>
#include <Graphics/Texture.h>
#include <Graphics/VAO.h>
#include <Graphics/VBO.h>
#include <Graphics/EBO.h>

namespace Engine {
	class Renderer : public Base {
	public:
		explicit Renderer(const RendererDesc& desc);
		~Renderer();

		void draw();

	private:
		Window& m_window;

		Shader m_shader;
		Texture m_texture;
		VBO m_VBO;
		EBO m_EBO;
		VAO m_VAO;

		GLuint m_uniID;
		GLuint m_tex0uni;
	};
}