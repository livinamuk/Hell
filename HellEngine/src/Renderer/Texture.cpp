#include "hellpch.h"
#include "Texture.h"
#include "DDS_Helpers.h"

namespace
{
	void freeCMPTexture(CMP_Texture* t)
	{
		free(t->pData);
	}

	constexpr uint32_t GL_COMPRESSED_RGB_S3TC_DXT1_EXT = 0x83F0;
	constexpr uint32_t GL_COMPRESSED_RGBA_S3TC_DXT1_EXT = 0x83F1;
	constexpr uint32_t GL_COMPRESSED_RGBA_S3TC_DXT3_EXT = 0x83F2;
	constexpr uint32_t GL_COMPRESSED_RGBA_S3TC_DXT5_EXT = 0x83F3;

	uint32_t cmpToOpenGlFormat(CMP_FORMAT format)
	{
		
		if (format == CMP_FORMAT_DXT1)
		{
			return GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
		}
		else if (format == CMP_FORMAT_DXT3)
		{
			return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		}
		else if (format == CMP_FORMAT_DXT5)
		{
			return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		}
		else
		{
			return 0xFFFFFFFF;
		}
	}
}

Texture::Texture(std::string name, std::string filetype)
{
	this->name = name;
	this->filetype = filetype;
}

void Texture::ReadFromDisk()
{
	//std::cout << "about to load tex ...." << name << "\n";
	std::string fullpathdds = "res/textures/" + name + ".dds";

	if (false && LoadDDSFile(fullpathdds.c_str(), m_cmpTexture))
	{
		m_isCompressed = true;
	}
	else
	{
		m_isCompressed = false;

		stbi_set_flip_vertically_on_load(false);
		std::string fullpath = "res/textures/" + name + "." + filetype;
		data = stbi_load(fullpath.c_str(), &width, &height, &nrChannels, 0);
	}
	m_readFromDisk = true;
	//std::cout << "Read from disk: " << name << "\n";
}

void Texture::LoadToGL()
{
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);
	

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (!m_isCompressed)
	{
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		GLint format = GL_RGB;

		if (nrChannels == 4)
			format = GL_RGBA;
		if (nrChannels == 1)
			format = GL_RED;

		// Generate texture
		if (data) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
			std::cout << "Failed to load texture: " << name << "\n";

		stbi_image_free(data);
		m_loadedToGL = true;
	}
	else
	{
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		uint32_t glFormat = cmpToOpenGlFormat(m_cmpTexture.format);

		unsigned int blockSize = (glFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;

		if (glFormat != 0xFFFFFFFF)
		{
			uint32_t width = m_cmpTexture.dwWidth;
			uint32_t height = m_cmpTexture.dwHeight;
			uint32_t size1 = ((width + 3) / 4) * ((height + 3) / 4) * blockSize;
			uint32_t size2 = m_cmpTexture.dwDataSize;

			glCompressedTexImage2D(GL_TEXTURE_2D, 0, glFormat, m_cmpTexture.dwWidth, m_cmpTexture.dwHeight, 0, size2, m_cmpTexture.pData);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 1);
			glGenerateMipmap(GL_TEXTURE_2D);

			freeCMPTexture(&m_cmpTexture);
			m_cmpTexture = {};
			m_loadedToGL = true;
		}
	}
	//std::cout << "loaded to GL: " << name << "\n";
}