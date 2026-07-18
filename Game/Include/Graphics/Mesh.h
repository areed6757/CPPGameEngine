#pragma once
#include <Core/Common.h>
#include <Graphics/EBO.h>
#include <Graphics/VAO.h>
#include <Graphics/VBO.h>

namespace Engine {
	struct MeshDesc {
		Base base;
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
		GLSizei m_indexCount{};
	};
}