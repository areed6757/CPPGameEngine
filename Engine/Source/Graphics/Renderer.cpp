#include <Graphics/Renderer.h>

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"	FragColor = vec4(0.8f, 0.3f, 0.02f, 1.0f);\n"
"}\n\0";


GLfloat vertices[] =
{
    -0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f,
    0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f,
    0.0f, 0.5f * float(sqrt(3)) * 2 / 3, 0.0f
};

Engine::Renderer::Renderer(const RendererDesc& desc) : Base(desc.base), m_window(desc.window)
{
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    m_shaderProgram = glCreateProgram();
    glAttachShader(m_shaderProgram, vertexShader);
    glAttachShader(m_shaderProgram, fragmentShader);

    glLinkProgram(m_shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Vertex Array Object (VAO) stores pointers to 1+ VBO and tells OpenGL how to interpret them
    // Vertex Buffer Object (VBO) - actually normally an array of references not just this one reference integer, passed to GPU to process a buffer
    // ALWAYS generate VAO first!

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    // Store vertices in the buffer object
    // the last arg can be STATIC_, STREAM_, DYNAMIC_ and _DRAW, _READ, _COPY
    // _STATIC_ is drawn once and used many many times
    // _STREAM_ is drawn once and used a few times
    // _DYNAMIC_ is modified many times and used many times
    // _DRAW means the vertices will be modified and used to draw an image on the screen
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // Vertex attribute is a way of communicating with a Vertex shader from the outside
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Unbind to prevent changing buffers VBO AND VAO with a function
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Engine::Renderer::~Renderer()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteProgram(m_shaderProgram);
}

void Engine::Renderer::draw()
{
    glClearColor( 0.1f, 0.1f, 0.1f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT );

    glUseProgram( m_shaderProgram );
    glBindVertexArray( m_VAO );
    glDrawArrays( GL_TRIANGLES, 0, 3 );

    glfwSwapBuffers( m_window.get() );

}






