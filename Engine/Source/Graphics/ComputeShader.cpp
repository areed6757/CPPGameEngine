#include <Graphics/ComputeShader.h>

namespace Engine {
	ComputeShader::ComputeShader(const ComputeShaderDesc& desc) : Base(desc.base)
	{
		std::string src = get_file_contents(desc.computeFile);
		const char* srcPtr = src.c_str();
		GLuint shader = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(shader, 1, &srcPtr, NULL);
		glCompileShader(shader);

		GLint compiled;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		if (!compiled) {
			char log[1024];
			glGetShaderInfoLog(shader, 1024, NULL, log);
			EngineLogErrorAndThrow("Compute shader compile failed: {}", log);
		}

		ID = glCreateProgram();
		glAttachShader(ID, shader);
		glLinkProgram(ID);
		glDeleteShader(shader);
	}

	ComputeShader::~ComputeShader()
	{
		glDeleteProgram(ID);
	}
	void ComputeShader::Activate()
	{
		glUseProgram(ID);
	}

	std::string ComputeShader::get_file_contents(const char* filename)
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
}