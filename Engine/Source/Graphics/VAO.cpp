#include <Graphics/VAO.h>

Engine::VAO::VAO()
{
	glGenVertexArrays(1, &ID);
}

Engine::VAO::~VAO() {
	glDeleteVertexArrays(1, &ID);
}

void Engine::VAO::LinkAttrib(const VBO& vbo, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset)
{
	vbo.Bind();
	glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
	glEnableVertexAttribArray(layout);
	vbo.Unbind();
}

void Engine::VAO::Bind() const
{
	glBindVertexArray(ID);
}

void Engine::VAO::Unbind() const
{
	glBindVertexArray(0);
}
