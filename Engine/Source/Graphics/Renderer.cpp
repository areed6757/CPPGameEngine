#include <Graphics/Renderer.h>
#include <format>

Engine::Renderer::Renderer(const RendererDesc& desc) : Base(desc.base),
    m_window(desc.window),
    m_shader(desc.shaderDesc),
    m_uniID()
{

    m_uniID = glGetUniformLocation(m_shader.ID, "scale");
    m_tex0uni = glGetUniformLocation(m_shader.ID, "tex0");


}

Engine::Renderer::~Renderer()
{
}

void Engine::Renderer::draw(const Mesh& mesh, const Texture& texture)
{
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT );

    m_shader.Activate();
    glUniform1i(m_tex0uni, 0);
    glUniform1f(m_uniID, 0.5f);
    
    texture.Bind();
    mesh.Bind();

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers( m_window.get() );
}