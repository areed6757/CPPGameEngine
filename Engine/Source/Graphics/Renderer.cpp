#include <Graphics/Renderer.h>

GLfloat vertices[] =
{
    -0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f,    // Lower left corner
    0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f,     // Lower right corner
    0.0f, 0.5f * float(sqrt(3)) * 2 / 3, 0.0f,  // Upper corner
    -0.5f /2, 0.5f * float(sqrt(3) / 6), 0.0f,  // Inner left
    0.5f /2, 0.5f * float(sqrt(3) / 6), 0.0f,   // Inner right
    0.0f, -0.5f * float(sqrt(3) / 3), 0.0f      // Inner down
};

GLuint indices[] =
{
    0, 3, 5, // Lower left triangle
    3, 2, 4, // Lower right triangle
    5, 4, 1  // Upper triangle
};


Engine::Renderer::Renderer(const RendererDesc& desc) : Base(desc.base),
    m_window(desc.window),
    m_shader("default.vert", "default.frag"),
    m_VAO(),
    m_VBO(vertices, sizeof(vertices)),
    m_EBO(indices, sizeof(indices))
{

    m_VAO.Bind();
    m_EBO.Bind();

    m_VAO.LinkVBO(m_VBO, 0);

    m_VAO.Unbind();
    m_VBO.Unbind();
    m_EBO.Unbind();
}

Engine::Renderer::~Renderer()
{
    m_VAO.Delete();
    m_VBO.Delete();
    m_EBO.Delete();
    m_shader.Delete();
}

void Engine::Renderer::draw()
{
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT );

    m_shader.Activate();
    m_VAO.Bind();

    glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers( m_window.get() );
}






