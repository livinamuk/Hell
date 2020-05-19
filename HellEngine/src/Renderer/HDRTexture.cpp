#include "hellpch.h"
#include "HDRTexture.h"

HDRTexture::HDRTexture()
{
}

HDRTexture::HDRTexture(std::string name, std::string filetype)
{
	this->name = name;
	this->filetype = filetype;

	stbi_set_flip_vertically_on_load(true);
	std::string fullpath = "res/HDR/" + name + "." + filetype;
	data = stbi_loadf(fullpath.c_str(), &width, &height, &nrComponents, 0);
	
	if (data)
	{
		glGenTextures(1, &ID);
		glBindTexture(GL_TEXTURE_2D, ID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
		std::cout << "Failed to load HDR image." << std::endl;
}