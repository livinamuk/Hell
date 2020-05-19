#pragma once
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <stb_image.h>

class HDRTexture
{
public: // Methods
	HDRTexture(std::string filename, std::string filetype);
	HDRTexture();

public: // fields
	unsigned int ID = 0;
	std::string name;
	std::string filetype;
	
private: // fields
	float* data;
	int width, height, nrComponents;
};

