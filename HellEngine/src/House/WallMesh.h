#pragma once
#include "Header.h"
//#include "WallSegment.h"

namespace HellEngine
{
	class WallMesh
	{
	public: // methods
		WallMesh();
		~WallMesh();
		void Draw(Shader* shader);
		void ClearMesh();
		void AddQuad(glm::vec3 cornerBottomLeft, glm::vec3 cornerTopRight, Axis axis);
		void AddPlane(glm::vec3 vertA, glm::vec3 vertB, glm::vec3 vertC, glm::vec3 vertD);
		void BufferMeshToGL(); 
		//void BuildFromWallSegments(std::vector<WallSegment> wallSegments);

	public: // fields
		unsigned int VAO = 0;
		unsigned int VBO, EBO;
		Transform transform;
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
	};
}