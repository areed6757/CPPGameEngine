#pragma once
#include <Core/Common.h>
#include <Graphics/TextureID.h>
#include <Graphics/Texture.h>
#include <array>

namespace Engine {
	struct TextureRegistryDesc {
		BaseDesc base;
	};

	class TextureRegistry : public Base {
	public:
		explicit TextureRegistry(const TextureRegistryDesc& desc);
		~TextureRegistry();

		[[nodiscard]] const Texture& get(TextureID id) const;

	private:
		std::array<Texture, static_cast<size_t>(TextureID::Count)> m_textures;
	};
}