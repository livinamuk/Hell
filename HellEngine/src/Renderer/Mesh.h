#pragma once

#include "Header.h"
#include "Renderer/Transform.h"
#include "Renderer/Shader.h"

namespace HellEngine
{
	class Mesh 
	{
	public: // methods
		Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, const char* name);
		void Draw();
		void SetupMesh();

	public: // fields
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		unsigned int VAO = 0;
		const char* name;

	private: // fields
		unsigned int VBO, EBO;
	};
}