#include <Graphics/Renderer.h>
#include <format>

GLfloat vertices[] =
{
    //  Coordinates             Colors
    -0.5f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f,   // Lower left corner
    -0.5f, 0.5f, 0.0f,      0.0f, 1.0f, 0.0f,   // Upper left corner
    0.5f, 0.5f, 0.0f,       0.0f, 0.0f, 1.0f,   // Upper right corner
    0.5f, -0.5f, 0.0f,      1.0f, 1.0f, 1.0f    // Lower left corner
};

GLuint indices[] =
{
    0, 2, 1, // Upper left triangle
    0, 3, 2  // Lower right triangle
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

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers( m_window.get() );
}