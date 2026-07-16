#pragma once
#include <Core/Common.h>
#include <ThirdParty/glad/glad.h>
#include <ThirdParty/stb/stb_image.h>

namespace Engine {
	class Texture : public Base {
	public:
		explicit Texture(const TextureDesc& desc);
		~Texture();

		void Bind() const;
		void Unbind() const;

	private:
		i32 m_widthImg;
		i32 m_heightImg;
		i32 m_colorChannels;

		unsigned char* m_bytes;

		GLuint m_texture;
	};
}