#pragma once
#include <Engine.h>
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