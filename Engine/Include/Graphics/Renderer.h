#pragma once
#include <Core/Common.h>
#include <Graphics/Window.h>

namespace Engine {
	class Renderer : public Base {
	public:
		explicit Renderer(const RendererDesc& desc);
		~Renderer();

		void draw();

	private:
		Window& m_window;
		GLuint m_VBO{}, m_VAO{}, m_shaderProgram{};
	};
}