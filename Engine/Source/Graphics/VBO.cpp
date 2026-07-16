#include <Graphics/VBO.h>


Engine::VBO::VBO(GLfloat* vertices, GLsizeiptr size) {
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

Engine::VBO::~VBO() {
	glDeleteBuffers(1, &ID);
}

void Engine::VBO::Bind() const {
	glBindBuffer(GL_ARRAY_BUFFER, ID);
}

void Engine::VBO::Unbind() const {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}