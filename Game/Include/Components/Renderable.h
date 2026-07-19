#pragma once
#include <Graphics/MeshID.h>
#include <Graphics/TextureID.h>
#include <optional>

namespace Engine {
	struct Renderable {
		MeshID mesh = MeshID(0);
		std::optional<TextureID> texture = TextureID(0);

		// tint scale etc, added here
	};
}