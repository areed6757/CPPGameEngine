#pragma once
#include<ThirdParty/glad/glad.h>

namespace Engine {
	class VBO {
	public:
		GLuint ID;
		VBO(const GLfloat* vertices, GLsizeiptr size);
		VBO();
		~VBO();

		void Bind() const;
		void Unbind() const;
		void BufferData(GLsizeiptr size, const void* data, GLenum usage = GL_DYNAMIC_DRAW) const;
	};
}