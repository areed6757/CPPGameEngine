#pragma once
#include<ThirdParty/glad/glad.h>

namespace Engine{
	class EBO {
	public:
		GLuint ID{};
		EBO(GLuint* indices, GLsizeiptr size);
		~EBO();

		void Bind() const;
		void Unbind() const;
	};
}