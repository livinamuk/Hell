#include "hellpch.h"
#include "Model.h"
#include "Helpers/AssetManager.h"
#include "Helpers/Util.h"

#include <assert.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

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
		/*if (m_fileType == FileType::FBX)
			Importer::LoadFbxModel(this);*/

		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, this->m_filePath.c_str())) {
			//throw std::runtime_error(warn + err);
		}

		std::unordered_map<Vertex, uint32_t> uniqueVertices = {};

		for (const auto& shape : shapes)
		{
			std::vector<Vertex> vertices;
			std::vector<unsigned int> indices;

			for (const auto& index : shape.mesh.indices) {
				Vertex vertex = {};

				vertex.Position = {
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2]
				};

				vertex.TexCoords = {
					attrib.texcoords[2 * index.texcoord_index + 0],
					1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
				};

				/*vertex.Normal = {
					attrib.vertices[3 * index.normal_index + 0],
					attrib.vertices[3 * index.normal_index + 1],
					attrib.vertices[3 * index.normal_index + 2]
				};*/

				//vertex.color = { 1.0f, 1.0f, 1.0f };

				if (uniqueVertices.count(vertex) == 0) {
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
				}

				indices.push_back(uniqueVertices[vertex]);
			}

			for (int i = 0; i < indices.size(); i += 3) {
				Util::SetNormalsAndTangentsFromVertices(&vertices[indices[i]], &vertices[indices[i + 1]], &vertices[indices[i + 2]]);

				//				Util::SetTangentsFromVertices(&vertices[indices[i]], &vertices[indices[i + 1]], &vertices[indices[i + 2]]);
			}

			//	std::cout << "verticies.size " << vertices.size() << "\n";
			//	std::cout << "indices.size " << indices.size() << "\n";

			Mesh* mesh = new Mesh(vertices, indices, shape.name.c_str());
			this->m_meshes.push_back(mesh);
			this->m_hasAnimation.push_back(false);
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