#pragma once
#include <Core/Common.h>
#include <Graphics/Window.h>

namespace Engine {
	class Renderer : public Base {
	public:
		explicit Renderer(const RendererDesc& desc);
		~Renderer();

	private:
		Window m_window;
	};
}