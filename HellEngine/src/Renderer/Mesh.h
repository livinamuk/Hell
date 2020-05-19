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
		Transform& GetTransform();
		void Draw();
		void Optimise();
		void SetupMesh();

	public: // fields
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		//Transform transform;
		unsigned int VAO = 0;
		const char* name;
		unsigned int materialID = 0;
		std::vector<int> linkedNodeIndices;

	private: // fields
		unsigned int VBO, EBO;
	};
}