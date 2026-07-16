#include <Graphics/Renderer.h>
#include <format>

GLfloat vertices[] =
{
    -0.5f, -0.5f * float(sqrt(3)) / 3,       0.0f, 0.8f, 0.3f, 0.02f,      // Lower left corner
    0.5f, -0.5f * float(sqrt(3)) / 3,        0.0f, 0.8f, 0.3f, 0.02f,      // Lower right corner
    0.0f, 0.5f * float(sqrt(3)) * 2 / 3,     0.0f, 1.0f, 0.6f, 0.32f,      // Upper corner
    -0.5f /2, 0.5f * float(sqrt(3) / 6),     0.0f, 0.9f, 0.45f, 0.17f,     // Inner left
    0.5f /2, 0.5f * float(sqrt(3) / 6),      0.0f, 0.9f, 0.45f, 0.17f,     // Inner right
    0.0f, -0.5f * float(sqrt(3) / 3),        0.0f, 0.8f, 0.3f, 0.02f       // Inner down
};

GLuint indices[] =
{
    0, 3, 5, // Lower left triangle
    3, 2, 4, // Lower right triangle
    5, 4, 1  // Upper triangle
};


Engine::Renderer::Renderer(const RendererDesc& desc) : Base(desc.base),
    m_window(desc.window),
    m_shader(desc.shaderDesc),
    m_VAO(),
    m_VBO(vertices, sizeof(vertices)),
    m_EBO(indices, sizeof(indices)),
    m_uniID()
{

    m_VAO.Bind();
    m_EBO.Bind();

    m_VAO.LinkAttrib(m_VBO, 0, 3, GL_FLOAT, 6* sizeof(float), (void*)0);
    m_VAO.LinkAttrib(m_VBO, 1, 3, GL_FLOAT, 6* sizeof(float), (void*)(3 * sizeof(float)));

    m_VAO.Unbind();
    m_VBO.Unbind();
    m_EBO.Unbind();

    m_uniID = glGetUniformLocation(m_shader.ID, "scale");
}

Engine::Renderer::~Renderer()
{
}

void Engine::Renderer::draw()
{
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT );

    m_shader.Activate();
    glUniform1f(m_uniID, 0.5f);

    m_VAO.Bind();

    glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers( m_window.get() );
}