#include <Graphics/Renderer.h>
#include <format>

Engine::Renderer::Renderer(const RendererDesc& desc) : Base(desc.base),
    m_window(desc.window),
    m_shader(desc.shaderDesc),
    m_camera(desc.camera)
{

    m_projectionUni = glGetUniformLocation(m_shader.ID, "projection");
    m_modelUni = glGetUniformLocation(m_shader.ID, "model");
    m_tex0uni = glGetUniformLocation(m_shader.ID, "tex0");
    m_useTextureUni = glGetUniformLocation(m_shader.ID, "useTexture");

}

Engine::Renderer::~Renderer()
{
}

void Engine::Renderer::beginFrame()
{
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    m_shader.Activate();

    glm::mat4 projection = m_camera.getProjection(m_window.getWidth(), m_window.getHeight());
    glUniformMatrix4fv(m_projectionUni, 1, GL_FALSE, &projection[0][0]);
}

void Engine::Renderer::draw(const Mesh& mesh, const Texture* texture, const glm::mat4& model)
{
    glUniformMatrix4fv(m_modelUni, 1, GL_FALSE, &model[0][0]);
    glUniform1i(m_useTextureUni, texture != nullptr);
    
    if (texture) {
        glUniform1i(m_tex0uni, 0);
        texture->Bind();
    }

    mesh.Bind();

    glDrawElements(GL_TRIANGLES, mesh.indexCount(), GL_UNSIGNED_INT, 0);
}

void Engine::Renderer::endFrame()
{
    glfwSwapBuffers(m_window.get());
}
