#pragma once
#include <Core/Common.h>
#include <Graphics/MeshID.h>
#include <array>

namespace Engine {
	struct MeshRegistryDesc {
		Base base;
	};

	// Private namespace
	namespace {
		GLfloat quadVertices[] = {
			//  Coordinates             Colors          Texture Coords
			-0.5f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f,   0.0f, 0.0f,   // Lower left corner
			-0.5f, 0.5f, 0.0f,      0.0f, 1.0f, 0.0f,   0.0f, 1.0f,   // Upper left corner
			0.5f, 0.5f, 0.0f,       0.0f, 0.0f, 1.0f,   1.0f, 1.0f,   // Upper right corner
			0.5f, -0.5f, 0.0f,      1.0f, 1.0f, 1.0f,   1.0f, 0.0f    // Lower right corner
		};
		GLuint quadIndices[] = {
			0, 2, 1, // Upper left triangle
			0, 3, 2  // Lower right triangle
		};
		GLfloat triangleVertices[] = {
			-0.5f, -0.5f, 0.0f,		1.0f, 0.0f, 0.0f,	0.0f, 0.0f,
			0.5f, -0.5f, 0.0f,		0.0f, 1.0f, 0.0f,	1.0f, 0.0f,
			0.0f, 0.5f, 0.0f,		0.0f, 0.0f, 1.0f,	0.5f, 1.0f
		};
		GLuint triangleIndices[] = {
			0, 1, 2
		};
	}

	class MeshRegistry : public Base {
	public:
		explicit MeshRegistry(const MeshRegistryDesc& desc);
		~MeshRegistry();

		[[nodiscard]] const Mesh& get(MeshID id) const;

	private:
		std::array<Mesh, static_cast<size_t>(MeshID::Count)> m_meshes;
	};
}