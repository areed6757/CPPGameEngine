#include <Graphics/EBO.h>


Engine::EBO::EBO(const GLuint* indices, GLsizeiptr size) {
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
}

Engine::EBO::~EBO() {
	glDeleteBuffers(1, &ID);
}

void Engine::EBO::Bind() const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
}

void Engine::EBO::Unbind() const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}