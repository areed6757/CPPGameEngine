#pragma once
#include <Graphics/MeshID.h>
#include <Graphics/TextureID.h>
#include <optional>

namespace Engine {
	struct Renderable {
		MeshID mesh = MeshID(0);
		std::optional<TextureID> texture = TextureID(0);
		f32 scale{ 1.0f };
		f32 rotationOffset{ 0.0f };

		// If set, RenderSystem swaps to this texture (at a floored world-space scale) once the entity's on-screen diameter drops below iconMinPixelSize
		// zero/nullopt disables the swap for this entity.
		std::optional<TextureID> iconTexture{};
		f32 iconMinPixelSize{ 0.0f };
	};
}