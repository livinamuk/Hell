#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "../Renderer/Mesh.h"
#include "fbxsdk.h"
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
		void CalculateAnimation(std::string animName, FbxTime currentTime);
		void SetMeshMaterialByName(const char* meshName, unsigned int materialID);
		void SetMaterial(unsigned int materialID);
		void GetSceneCameraVectors(fbxsdk::FbxVector4& pos, fbxsdk::FbxVector4& lookAt, fbxsdk::FbxVector4& upVec) const;
		glm::mat4 GetSceneCameraMatrix();
	
	public:
		std::vector<Mesh*> m_meshes;
		std::unordered_map<std::string, Animation> m_animations;

	public:
		std::string name;
		std::string m_filePath;
		bool m_readFromDisk = false;
		bool m_loadedToGL = false;
		FileType m_fileType;

		std::vector<fbxsdk::FbxAMatrix> m_nodeBlendMatrices;
		std::vector<bool> m_hasAnimation;

		std::vector<std::vector<fbxsdk::FbxAMatrix>> m_clusterInitialMats;
		std::vector<std::vector<fbxsdk::FbxAMatrix>> m_clusterRelativeInitMats;
		std::vector<std::vector<std::string>>		 m_clusterLinkNames;

		std::unordered_map<std::string, int> m_nodeNameMap;

		fbxsdk::FbxManager* m_fbxMgr{ nullptr };

		fbxsdk::FbxCamera* m_sceneCamera{ nullptr };
		fbxsdk::FbxVector4 m_sceneCamPos;
		fbxsdk::FbxVector4 m_sceneCamLookAt;
		fbxsdk::FbxVector4 m_sceneCamUpVec;
	};
}