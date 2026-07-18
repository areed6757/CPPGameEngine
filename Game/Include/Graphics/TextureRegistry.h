#pragma once
#include <Core/Common.h>
#include <Graphics/TextureID.h>

namespace Engine {
	struct TextureRegistryDesc {
		Base base;
	};

	class TextureRegistry : public Base {
	public:
		explicit TextureRegistry(const TextureRegistryDesc& desc);
		~TextureRegistry();



	};
}