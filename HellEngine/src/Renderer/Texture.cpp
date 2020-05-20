#include "hellpch.h"
#include "Texture.h"

Texture::Texture(std::string name, std::string filetype)
{
	this->name = name;
	this->filetype = filetype;
}

void Texture::ReadFromDisk()
{
	//std::cout << "about to load tex ...." << name << "\n";
	stbi_set_flip_vertically_on_load(false);
	std::string fullpath = "res/textures/" + name + "." + filetype;
	data = stbi_load(fullpath.c_str(), &width, &height, &nrChannels, 0);
	m_readFromDisk = true;
	std::cout << "Read from disk: " << name << "\n";
}

void Texture::LoadToGL()
{
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

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
	//std::cout << "loaded to GL: " << name << "\n";
}