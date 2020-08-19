#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "../Renderer/Mesh.h"
#include "Header.h"

namespace HellEngine
{
	class Model
	{
	public:
		Model(const char* filepath);
		~Model();

		void ReadFromDisk();
		void LoadMeshDataToGL();
		void Draw(Shader* shader, glm::mat4 modelMatrix);
		void DrawMesh(Shader* shader, int meshIndex, glm::mat4 modelMatrix);
		//void SetMeshMaterialByName(const char* meshName, unsigned int materialID);
		//void SetMaterial(unsigned int materialID);
		void Set_Mesh_MaterialID_Set0(const char* meshname, unsigned int materialID);
		void Set_Mesh_MaterialID_Set1(const char* meshname, unsigned int materialID);
	
	public:
		std::vector<Mesh*> m_meshes;

	public:
		std::string name;
		std::string m_filePath;
		bool m_readFromDisk = false;
		bool m_loadedToGL = false;
		FileType m_fileType;
	};
}