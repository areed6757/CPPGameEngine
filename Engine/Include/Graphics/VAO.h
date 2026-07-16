#ifndef VAO_CLASS_H
#define VAO_CLASS_H

#include <ThirdParty/glad/glad.h>
#include <Graphics/VBO.h>

class VAO {
public:
	GLuint ID;
	VAO();

	void LinkVBO(VBO VBO, GLuint layout);
	void Bind();
	void Unbind();
	void Delete();
};

#endif