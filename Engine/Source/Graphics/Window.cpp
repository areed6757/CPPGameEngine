#include <Graphics/Window.h>
#include <Input/InputHandler.h>

// Wrapper to handle creation and destruction of OpenGL windows via GLFW/glad
Engine::Window::Window(const WindowDesc& desc) : Base(desc.base){

    /// TODO: Remove and create a new class for these later controlled by GraphicsSystem in ECS. Shaders will later be passed to the window.
    // Temporary copy of vertex and fragment shader from tutorial. TODO: analyze and understand these, maybe rewrite if needed.
    //           (1)
    //            |
    //      (-1)--|--(1)
    //            |
    //           (-1)
    // Standard coordinate plane format for x and y, but z is also an axis, ignored here
    // plane origin is the center point of the window
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
        -0.5f, -0.5f * float(sqrt(3.0)) / 3.0, 0.0f,
        0.5f, 0.5f * float(sqrt(3.0)) / 3.0, 0.0f,
        0.0f, 0.5f * float(sqrt(3.0)) * 2.0 / 3.0, 0.0f
    };

    /// CONTINUE WINDOW CLASS

    GLFWwindow* rawWindow(glfwCreateWindow(desc.windowWidth, desc.windowHeight, desc.title, NULL, NULL));
    if (!rawWindow) {
        EngineLogErrorAndThrow("GLFW window creation failed.");
    }

    m_window.reset(rawWindow);

    glfwMakeContextCurrent(m_window.get());

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        EngineLogErrorAndThrow("Glad initiation failed.");
    }

    if (!m_window.get()) {
        EngineLogErrorAndThrow("GLFW window creation failed.");
        glfwTerminate();
    }

    glfwSwapInterval(1); // int frames to wait to swap buffers (basically vsync)

    // Input handling - all inputs to a window will be passed through
    InputHandlerDesc handleDesc{ BaseDesc{desc.base.logger}, desc.actionMap };
    m_inputHandler = std::make_unique<InputHandler>(handleDesc);

    if (!m_inputHandler.get()) {
        EngineLogErrorAndThrow("Input handler not created for Window.");
    }

    // Window decoration "thicknesses"
    i32 left, top, right, bottom;
    glfwGetWindowFrameSize(m_window.get(), &left, &top, &right, &bottom);

    // Frame buffer size, TODO: handle window resizing via callback
    i32 width, height;
    glfwGetFramebufferSize(m_window.get(), &width, &height);
    glViewport(0, 0, width, height); // From x = 0, y = 0 -> x = width, y = height


    /// MOVE THIS WHEN MOVING THE TOP BLOCK IN THIS CLASS
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Vertex Buffer Object (VBO) - actually normally an array of references not just this one reference integer, passed to GPU to process a buffer
    // Vertex Array Object (VAO) stores pointers to 1+ VBO and tells OpenGL how to interpret them
    // ALWAYS generate VAO first!

    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
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

    // Where to put these?
    // glDeleteVertexArrays(1, &VAO);
    // glDeleteBuffers(1, &VBO);
    // glDeleteProgram(shaderProgram);
    /// RESUME WINDOW CLASS


    // Static color render
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);    // Specify color
    glClear(GL_COLOR_BUFFER_BIT);               // Assign color to a cleaned back buffer
    glfwSwapBuffers(m_window.get());            // swap back buffer with front

    glfwSetWindowUserPointer(m_window.get(), this);
    glfwSetKeyCallback(m_window.get(), Window::key_callback);

    EngineLogInfo("GLFW window created.");
}

Engine::Window::~Window()
{
    EngineLogInfo("Window closing...");
}

bool Engine::Window::shouldClose() const noexcept
{
    return glfwWindowShouldClose(m_window.get());
}

GLFWwindow* Engine::Window::get() const noexcept
{
    return m_window.get();
}

// Static key callback function for input handling, prompts the InputHandler associated with this Window.
void Engine::Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (self && self->m_inputHandler) {
        self->m_inputHandler->onKey(key, scancode, action, mods);
    }
}