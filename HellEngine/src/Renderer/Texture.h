#pragma once
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <stb_image.h>

#include "DDS_Helpers.h"


class Texture
{
public: // Methods
	Texture(std::string filename, std::string filetype);

	void ReadFromDisk();
	void LoadToGL();

public: // fields
	unsigned int ID = 0;
	std::string name;
	std::string filetype;
	bool m_readFromDisk = false;
	bool m_loadedToGL = false;
	int width, height;

private: // fields
	CMP_Texture				  m_cmpTexture = {};
	bool					  m_isCompressed = false;
	unsigned char* data		  = nullptr;
	int nrChannels;
};

