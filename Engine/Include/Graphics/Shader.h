#pragma once
#include <Core/Common.h>
#include <ThirdParty/glad/glad.h>
#include <ThirdParty/stb/stb_image.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

namespace Engine {
	class Shader : public Base {
	public:
		explicit Shader(const ShaderDesc& desc);
		~Shader();

		GLuint ID;

		void Activate();
	
		void compileErrors(unsigned int shader, const char* type);

	private:
		const char* m_vertexFile;
		const char* m_fragmentFile;

		std::string get_file_contents(const char* filename);
	};
}