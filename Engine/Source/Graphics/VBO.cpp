#include <Graphics/VBO.h>

namespace Engine {
	VBO::VBO(const GLfloat* vertices, GLsizeiptr size) {
		glGenBuffers(1, &ID);
		glBindBuffer(GL_ARRAY_BUFFER, ID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}

	VBO::VBO() {
		glGenBuffers(1, &ID);
	};

	VBO::~VBO() {
		glDeleteBuffers(1, &ID);
	}

	void VBO::Bind() const {
		glBindBuffer(GL_ARRAY_BUFFER, ID);
	}

	void VBO::Unbind() const {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	void VBO::BufferData(GLsizeiptr size, const void* data, GLenum usage) const
	{
		Bind();
		glBufferData(GL_ARRAY_BUFFER, size, data, usage);
		Unbind();
	}
}