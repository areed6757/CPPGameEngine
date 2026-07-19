#pragma once
#include <Graphics/MeshID.h>
#include <Graphics/TextureID.h>
#include <optional>

namespace Engine {
	struct Renderable {
		MeshID mesh = MeshID(0);
		std::optional<TextureID> texture = TextureID(0);
		f32 scale{ 1.0f };

		// tint scale etc, added here
	};
}