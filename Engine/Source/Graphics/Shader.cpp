#include <Graphics/Shader.h>

Engine::Shader::Shader(const ShaderDesc& desc) : Base(desc.base),
    m_vertexFile(desc.vertexFile), m_fragmentFile(desc.fragmentFile)
{
	std::string vertexCode = get_file_contents(m_vertexFile);
	std::string fragmentCode = get_file_contents(m_fragmentFile);

	const char* vertexSource = vertexCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);
    compileErrors(vertexShader, "VERTEX");

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);
    compileErrors(fragmentShader, "FRAGMENT");

    ID = glCreateProgram();
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);

    glLinkProgram(ID);
    compileErrors(ID, "PROGRAM");

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

Engine::Shader::~Shader()
{
    glDeleteProgram(ID);
}

void Engine::Shader::Activate()
{
    glUseProgram(ID);
}

std::string Engine::Shader::get_file_contents(const char* filename)
{
	std::ifstream in(filename, std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
    EngineLogErrorAndThrow("Failed to load file: {}", filename);
}

void Engine::Shader::compileErrors(unsigned int shader, const char* type) {
    GLint hasCompiled;
    char infoLog[1024];
    if (strcmp(type, "PROGRAM") != 0) {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
        if (hasCompiled == GL_FALSE) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            EngineLogErrorAndThrow("SHADER_COMPILATION_ERROR for: {}", type);
        }
    }
    else {
        glGetProgramiv(shader, GL_COMPILE_STATUS, &hasCompiled);
        if (hasCompiled == GL_FALSE) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            EngineLogErrorAndThrow("SHADER_LINKING_ERROR for: {}", type);
        }
    }
}