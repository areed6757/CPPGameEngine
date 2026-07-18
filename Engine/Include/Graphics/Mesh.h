#pragma once
#include <Core/Common.h>
#include <Graphics/EBO.h>
#include <Graphics/VAO.h>
#include <Graphics/VBO.h>
#include <ThirdParty/glad/glad.h>

namespace Engine {
	struct MeshDesc {
		BaseDesc base;
		const GLfloat* vertices;
		GLsizeiptr verticesSize;
		const GLuint* indices;
		GLsizeiptr indicesSize;
		GLsizei indexCount;
	};

	class Mesh : public Base {
	public:
		explicit Mesh(const MeshDesc& desc);
		
		Mesh(const Mesh&) = delete;
		Mesh& operator=(const Mesh&) = delete;
		Mesh(Mesh&&) = default;		// Allow moves, one owner
		Mesh& operator=(Mesh&&) = default;

		void Bind() const;
		[[nodiscard]] GLsizei indexCount() const;

	private:
		VAO m_VAO;
		VBO m_VBO;
		EBO m_EBO;
		GLsizei m_indexCount{};
	};
}