#pragma once
#include<ThirdParty/glad/glad.h>

namespace Engine {
	class VBO {
	public:
		GLuint ID;
		VBO(GLfloat* vertices, GLsizeiptr size);
		~VBO();

		void Bind() const;
		void Unbind() const;
	};
}