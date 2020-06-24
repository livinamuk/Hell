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
		const char* name;
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		unsigned int VAO = 0;
		unsigned int VBO = 0;
		unsigned int EBO = 0;
		unsigned int baseVertex;
		unsigned int baseIndex;
	};
}