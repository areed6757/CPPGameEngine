#include <Graphics/TextureRegistry.h>

Engine::TextureRegistry::TextureRegistry(const TextureRegistryDesc& desc) :
	Base(desc.base),
	m_textures{
		// Must instantiate in order of TextureID struct
		Texture{ TextureDesc {desc.base, 512, 512, 4, "Images/test.png"}}
	}
{
	EngineLogInfo("Texture registry created.");
}

Engine::TextureRegistry::~TextureRegistry()
{
	EngineLogInfo("Texture registry destroyed.");
}

const Engine::Texture& Engine::TextureRegistry::get(TextureID id) const {
	return m_textures[static_cast<size_t>(id)];
}