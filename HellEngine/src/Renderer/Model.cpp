#include "hellpch.h"
#include "Model.h"
#include "Helpers/AssetManager.h"
#include "Helpers/Util.h"
#include "GL/GpuProfiling.h"

#include <assert.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

namespace HellEngine
{
	Model::Model(const char* filepath)
	{
		m_filePath = filepath;
		name = Util::FileNameFromPath(filepath);
		m_fileType = Util::FileTypeFromPath(filepath);
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

				//if (std::string("res/models/blood/blood1/blood_mesh.obj").compare(std::string(m_filePath)) == 0) {
				//	//std::cout << "\nvertices:" << vertices.size() << " " << indices.size() << "\n";
				//	vertex.Position /= 1000;
				//}

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

//			if (std::string("res/models/blood/blood1/blood_mesh.obj").compare(std::string(m_filePath)) == 0) {	
//				indices.clear();
//				indices = { 0, 1, 2, 0, 2, 3 };
//
//				vertices.clear();
//
//				float quadVertices[] = {
//-0.5f, +0.5f, +0.0f, 0.0f, 0.0f,
//+0.5f, +0.5f, +0.0f, 1.0f, 0.0f,
//+0.5f, -0.5f, +0.0f, 1.0f, 1.0f,
//-0.5f, -0.5f, +0.0f, 0.0f, 1.0f
//				};
//
//				for (int i = 0; i < 4; ++i) {
//					int kk = 5 * i;
//					Vertex vertex = {};
//					vertex.Position = { quadVertices[kk], quadVertices[kk + 1], quadVertices[kk + 2]};
//					vertex.TexCoords = { quadVertices[kk + 3], quadVertices[kk + 4]};
//
//					vertices.push_back(vertex);
//				}
//			}

			if (std::string("res/models/blood/blood1/blood_cube_test.obj").compare(std::string(m_filePath)) == 0) {
				std::cout << " rahat pe batz! " << vertices.size() << std::endl;
			}

			for (int i = 0; i < indices.size(); i += 3) {
				Util::SetNormalsAndTangentsFromVertices(&vertices[indices[i]], &vertices[indices[i + 1]], &vertices[indices[i + 2]]);
			}

			Mesh* mesh = new Mesh(vertices, indices, shape.name.c_str());

			m_meshes.push_back(mesh);
		}
		m_readFromDisk = true;
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
		GpuProfiler g("Model");
		for (int i = 0; i < m_meshes.size(); ++i)
			DrawMesh(shader, i, modelMatrix);
	}

	void Model::DrawMesh(Shader* shader, int meshIndex, glm::mat4 modelMatrix)
	{
		// If it aint loaded DO NOT TRY DRAW
		if (!m_loadedToGL && (shader->name.compare("BloodVolumetric") != 0)) return;
		if (m_meshes.size() <= 0) return;

		shader->setMat4("model", modelMatrix);

		m_meshes[meshIndex]->Draw();
	}

	
	/*void Model::SetMeshMaterialByName(const char* meshName, unsigned int materialID)
	{
		for (size_t i = 0; i < m_meshes.size(); i++)
			if (std::string(m_meshes[i]->name) == std::string(meshName))
				m_meshes[i]->materialID = materialID;
	}

	void Model::SetMaterial(unsigned int materialID)
	{
		for (size_t i = 0; i < m_meshes.size(); i++)
			m_meshes[i]->materialID = materialID;
	}*/
}