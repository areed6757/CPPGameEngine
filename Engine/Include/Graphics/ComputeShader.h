#pragma once
#include <Core/Common.h>
#include <ThirdParty/glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

namespace Engine {
	struct ComputeShaderDesc {
		BaseDesc base;
		const char* computeFile;
	};

	class ComputeShader : public Base {
	public :
		explicit ComputeShader(const ComputeShaderDesc& desc);
		~ComputeShader();

		GLuint ID;
		void Activate();

	private:
		std::string get_file_contents(const char* filename);
	};
}