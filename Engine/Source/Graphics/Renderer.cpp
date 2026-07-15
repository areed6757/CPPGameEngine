#include <Graphics/Renderer.h>

Engine::Renderer::Renderer(const RendererDesc& desc) : Base(desc.base), m_window(desc.window)
{
}

Engine::Renderer::~Renderer()
{
}
