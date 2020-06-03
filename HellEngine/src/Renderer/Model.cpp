#include "hellpch.h"
#include "Model.h"
#include "Helpers/AssetManager.h"
#include "Helpers/Importer.h"
#include "Helpers/Util.h"

#include <assert.h>
namespace HellEngine
{
	Model::Model(const char* filepath)
	{
		this->m_filePath = filepath;
		this->name = Util::FileNameFromPath(filepath);
		this->m_fileType = Util::FileTypeFromPath(filepath);
	}

	Model::~Model()
	{
		//for (uint32_t i = 0; i < m_meshes.size(); ++i)
		//	delete m_meshes[i];
		//m_meshes.clear();
	}

	void Model::ReadFromDisk()
	{
		//std::cout << "about to load model .... \n ";
		if (m_fileType == FileType::FBX)
			Importer::LoadFbxModel(this);

		else if (m_fileType == FileType::OBJ)
			Importer::LoadOBJModel(this);

		else {
			std::cout << "COULD NOT READ " << name << " (Unknown file format)\n";
			m_readFromDisk = true;
			return;
		}

		//std::cout << "Read model from disk: " << m_filePath << " (" << m_meshes.size() << ") meshes\n";
		m_readFromDisk = true;

		/*for (int i = 0; i < m_meshes.size(); i++)
		{
			std::cout << i << ": " << m_meshes[i]->name << "\n";
		}*/
	}

	void Model::LoadMeshDataToGL()
	{
		for (Mesh* mesh : m_meshes)
			mesh->SetupMesh();

		m_loadedToGL = true;
		//std::cout << "Loaded to GL: " << m_filePath << "\n";
	}


	void Model::Draw(Shader* shader, glm::mat4 modelMatrix)
	{
		for (int i = 0; i < m_meshes.size(); ++i)
			DrawMesh(shader, i, modelMatrix);
	}

	void Model::DrawMesh(Shader* shader, int meshIndex, glm::mat4 modelMatrix)
	{
		// If it aint loaded DO NOT TRY DRAW
		if (!m_loadedToGL) return;

		if (m_meshes.size() <= 0) return;

		shader->setBool("hasAnimation", m_hasAnimation[meshIndex]);
		if (m_hasAnimation[meshIndex])
		{
			glm::mat4 blendMatrices[64];
			for (int j = 0; j < m_clusterInitialMats[meshIndex].size(); ++j)
			{
				std::string const& linkName = m_clusterLinkNames[meshIndex][j];
				int const nodeIndex = m_nodeNameMap[linkName];
				fbxsdk::FbxAMatrix const& nodeMat = m_nodeBlendMatrices[nodeIndex];
				fbxsdk::FbxAMatrix finalMat = m_clusterInitialMats[meshIndex][j] * nodeMat * m_clusterRelativeInitMats[meshIndex][j];
				for (int k = 0; k < 16; ++k)
				{
					int x = k / 4, y = k % 4;
					blendMatrices[j][x][y] = (float)finalMat[x][y];
				}
			}
			shader->setMat4("skinningMats", &blendMatrices[0][0][0], 64);
		}
		shader->setMat4("model", modelMatrix);
		m_meshes[meshIndex]->Draw();
		shader->setBool("hasAnimation", 0);
	}

	void Model::CalculateAnimation(std::string name, FbxTime currentTime)
	{
		auto it = m_animations.find(name);
		if (it == m_animations.end()) {
			return;
		}
		//if (m_nodeBlendMatrices.size() == 0)
		//	return;

		Animation& anim = (*it).second;
		fbxsdk::FbxScene* scene = anim.m_scene;
		int const nodeCount = scene->GetNodeCount();

		fbxsdk::FbxAnimLayer* animLayer = nullptr;

		fbxsdk::FbxAMatrix lDummyGlobalPosition;
		fbxsdk::FbxPose* pose = nullptr;

		for (int i = 0; i < nodeCount; ++i)
		{
			fbxsdk::FbxNode* node = scene->GetNode(i);
			if (node->GetCamera() != nullptr)
			{
				m_sceneCamera = node->GetCamera();
			}
			std::string nodeName(node->GetNameOnly().Buffer());
			if (m_nodeNameMap.find(nodeName) == m_nodeNameMap.end())
			{
				continue;
			}
			int const index = m_nodeNameMap[nodeName];
			//m_nodeBlendMatrices[index] = node->e
			m_nodeBlendMatrices[index] = node->EvaluateGlobalTransform(currentTime);

			/*std::cout << node->GetName() << "\n";
			
			glm::mat4 m;
			for (int x = 0; x < 4; x++)
				for (int y = 0; y < 4; y++)
					m[x][y] = m_nodeBlendMatrices[index][x][y];

			Util::PrintMat4(m);
			std::cout << "\n";*/

		}

		if (m_sceneCamera)
		{
			m_sceneCamPos = m_sceneCamera->EvaluatePosition(currentTime);
			m_sceneCamLookAt = m_sceneCamera->EvaluateLookAtPosition(currentTime);
			m_sceneCamUpVec = m_sceneCamera->EvaluateUpDirection(m_sceneCamPos, m_sceneCamLookAt, currentTime);
		}	
	}

	void Model::SetMeshMaterialByName(const char* meshName, unsigned int materialID)
	{
		for (size_t i = 0; i < m_meshes.size(); i++)
			if (std::string(m_meshes[i]->name) == std::string(meshName))
				m_meshes[i]->materialID = materialID;
	}

	void Model::SetMaterial(unsigned int materialID)
	{
		for (size_t i = 0; i < m_meshes.size(); i++)
			m_meshes[i]->materialID = materialID;
	}

	void Model::GetSceneCameraVectors(fbxsdk::FbxVector4& pos, fbxsdk::FbxVector4& lookAt, fbxsdk::FbxVector4& upVec) const
	{
		pos = m_sceneCamPos;
		lookAt = m_sceneCamLookAt;
		upVec = m_sceneCamUpVec;
	}

	glm::mat4 Model::GetSceneCameraMatrix()
	{
		fbxsdk::FbxVector4 cp = m_sceneCamPos;// fbxsdk::FbxVector4(0, 0, 0, 1.0f);// m_sceneCamPos;
		fbxsdk::FbxVector4 cl = m_sceneCamLookAt;
		fbxsdk::FbxVector4 cu = m_sceneCamUpVec;

		return glm::lookAt(glm::vec3(cp[0], cp[1], cp[2]), glm::vec3(cl[0], cl[1], cl[2]), glm::vec3(cu[0], cu[1], cu[2]));
	}
}