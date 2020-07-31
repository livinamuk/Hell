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
//	std::string fullpath = "res/textures/" + name + "." + filetype;

	//std::cout << fullpath << "\n";

	/*if (filetype == "exr") {
		char* err;
		if (LoadEXR((float**)&data, &width, &height, fullpath.c_str(), (const char**)&err) < 0) {
			std::cout << "Error reading EXR file:" << err << "\n";
		}

		std::cout << "\nSUCCESS reading EXR file: " << fullpath << "\n";

		int num_layers;
		const char** layer_names;
		EXRLayers(fullpath.c_str(), &layer_names, &num_layers, (const char**)&err);

		std::cout << "num_layers: " << num_layers << "\n";
		std::cout << "layer_names: " << layer_names << "\n";
		std::cout << "err: " << err << "\n";

		for (int i = 0; i < num_layers; ++i) {
			std::cout << "layer_names[" << i << "]: " << layer_names[i] << "\n";
		}
	}
	else {
		stbi_set_flip_vertically_on_load(false);
		data = stbi_load(fullpath.c_str(), &width, &height, &nrChannels, 0);
	}
	m_readFromDisk = true;
	
	/*/
	
	//std::cout << "about to load tex ...." << name << "\n";
	stbi_set_flip_vertically_on_load(false);
	std::string fullpath = "res/textures/" + name + "." + filetype;
	data = stbi_load(fullpath.c_str(), &width, &height, &nrChannels, 0);
	m_readFromDisk = true;
	//std::cout << "Read from disk: " << name << "\n";*/
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
}