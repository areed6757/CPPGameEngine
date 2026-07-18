#include <Graphics/Texture.h>

Engine::Texture::Texture(const TextureDesc& desc) : Base(desc.base),
	m_heightImg(desc.heightImg),
	m_widthImg(desc.widthImg),
	m_colorChannels(desc.colorChannels),
	m_bytes(stbi_load(desc.imgAddr, &m_widthImg, &m_heightImg, &m_colorChannels, 4))
{
	glGenTextures(1, &m_texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_widthImg, m_heightImg, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_bytes);

	glGenerateMipmap(GL_TEXTURE_2D);
}

Engine::Texture::~Texture()
{
	glDeleteTextures(1, &m_texture);
	stbi_image_free(m_bytes);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Engine::Texture::Bind() const
{
	glBindTexture(GL_TEXTURE_2D, m_texture);
}

void Engine::Texture::Unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}
