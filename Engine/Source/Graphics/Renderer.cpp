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




Engine::Renderer::Renderer(const RendererDesc& desc) : Base(desc.base), m_window(desc.window)
{
    // Vertex shaders process point indices to determine where they should be placed on screen
    // (Also, they prepare lighting and texture coords for next step)
    // Create vertex shader and get its reference
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    // Attach vertex shader source to vertex shader object
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    // Compile vertex shader -> machine code
    glCompileShader(vertexShader);

    // Fragment shaders determine the exact color and appearance of every pixel on screen
    // Create fragment shader and get its reference
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    // Attach fragment shader source to vertex shader object
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    // Compile fragment shader -> machine code
    glCompileShader(fragmentShader);

    // Create shader program object, get ref
    m_shaderProgram = glCreateProgram();
    // Attach vertex+fragment shaders to the program
    glAttachShader(m_shaderProgram, vertexShader);
    glAttachShader(m_shaderProgram, fragmentShader);

    // Wrap-up/link all shaders into the program
    glLinkProgram(m_shaderProgram);

    // Delete the now useless vertex and fragment shader objects (they live in the program now)
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Vertex Array Object (VAO) stores pointers to 1+ VBO and tells OpenGL how to interpret them
    // Vertex Buffer Object (VBO) - actually normally an array of references not just this one reference integer, passed to GPU to process a buffer
    // ALWAYS generate VAO first!

    glGenVertexArrays(1, &m_VAO);   //make VAO w/ 1 object only
    glGenBuffers(1, &m_VBO);        //make VBO w/ 1 object only
    glGenBuffers(1, &m_EBO);
    glBindVertexArray(m_VAO);       // makes the VAO the current VAO by binding it
    
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO); // bind VBO, define its type
    // Store vertices in the buffer object
    // the last arg can be STATIC_, STREAM_, DYNAMIC_ and _DRAW, _READ, _COPY
    // _STATIC_ is drawn once and used many many times
    // _STREAM_ is drawn once and used a few times
    // _DYNAMIC_ is modified many times and used many times
    // _DRAW means the vertices will be modified and used to draw an image on the screen
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // Introduce vertices to the VBO
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

    // Vertex attribute is a way of communicating with a Vertex shader from the outside
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // Configs Vertex Attribute so OpenGL can read the VBO
    glEnableVertexAttribArray(0); // Enables Vertex Attribute so OpenGL knows to use it
    // Unbind to prevent changing buffers VBO AND VAO with a function
    glBindBuffer(GL_ARRAY_BUFFER, 0);   // Bind VBO and VAO to 0 to avoid accidentally modifying them
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Unbind EBO AFTER VAO, because EBO is stored in the VAO
}

Engine::Renderer::~Renderer()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);
    glDeleteProgram(m_shaderProgram);
}

void Engine::Renderer::draw()
{
    // Set a static color
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT );

    // Tell OpenGL which program to use
    glUseProgram( m_shaderProgram );
    // Bind VAO so it is used
    glBindVertexArray( m_VAO );
    // Draw object w/ GL_TRIANGLES primitive
    //glDrawArrays( GL_TRIANGLES, 0, 3 );

    // Uses the EBO to draw
    glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers( m_window.get() );

}






