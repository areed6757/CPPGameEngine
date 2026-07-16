#pragma once
#include <ThirdParty/glad/glad.h>
#include <Graphics/VBO.h>

namespace Engine {
	class VAO {
	public:
		GLuint ID;
		VAO();
		~VAO();

		void LinkAttrib(const VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset);
		void Bind() const;
		void Unbind() const;
	};
}
