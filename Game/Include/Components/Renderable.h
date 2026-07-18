#pragma once
#include <Graphics/MeshID.h>
#include <Graphics/TextureID.h>

namespace Engine {
	struct Renderable {
		MeshID mesh = MeshID(0);
		TextureID texture = TextureID(0);

		// tint scale etc, added here
	};
}