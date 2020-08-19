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
		std::cout << "\n\nLOADING: " << this->name;

		//std::cout << "about to load model .... \n ";
		/*if (m_fileType == FileType::FBX)
			Importer::LoadFbxModel(this);*/

		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		
		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, this->m_filePath.c_str())) {
			throw std::runtime_error(warn + err);
		}

	/*	std::cout << "\nmatetreials size: " << materials.size();
		for (int i = 0; i < materials.size(); i++)
		{
			std::cout << "\nmat: " << materials[i].name;
		}*/

		std::unordered_map<Vertex, uint32_t> uniqueVertices = {};


		/*

		// Loop over shapes
		for (size_t s = 0; s < shapes.size(); s++) {

			std::vector<Vertex> vertices;
			std::vector<unsigned int> indices;


			// Loop over faces(polygon)
			size_t index_offset = 0;
			for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
				int fv = shapes[s].mesh.num_face_vertices[f];

				// Loop over vertices in the face.
				for (size_t v = 0; v < fv; v++) {
					// access to vertex
					tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

					Vertex vertex = {};

					vertex.Position.x = attrib.vertices[3 * idx.vertex_index + 0];
					vertex.Position.y = attrib.vertices[3 * idx.vertex_index + 1];
					vertex.Position.z = attrib.vertices[3 * idx.vertex_index + 2];

					vertex.TexCoords.x = attrib.vertices[2 * idx.texcoord_index + 0];
					vertex.TexCoords.y = attrib.vertices[2 * idx.texcoord_index + 1];
				
					// per-face material
					vertex.MaterialID = shapes[s].mesh.material_ids[f];

					if (uniqueVertices.count(vertex) == 0) {
						uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
						vertices.push_back(vertex);
					}

					indices.push_back(uniqueVertices[vertex]);
				}

				index_offset += fv;

				
				shapes[s].mesh.material_ids[f];
			}


			for (int i = 0; i < indices.size(); i += 3) {
				Util::SetNormalsAndTangentsFromVertices(&vertices[indices[i]], &vertices[indices[i + 1]], &vertices[indices[i + 2]]);
			}
			std::cout << "\n indices.size(): " << indices.size();

			Mesh* mesh = new Mesh(vertices, indices, shapes[s].name.c_str());
			this->m_meshes.push_back(mesh);
		}*/


		for (const auto& shape : shapes)
		{
			std::vector<Vertex> vertices;
			std::vector<unsigned int> indices;

			for (int i=0; i < shape.mesh.indices.size(); i++) {
	//		for (const auto& index : shape.mesh.indices) {
				Vertex vertex = {};

				const auto& index = shape.mesh.indices[i];
				vertex.MaterialID = shape.mesh.material_ids[i/3];

				vertex.Position = {
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2]
				};

				vertex.TexCoords = {
					attrib.texcoords[2 * index.texcoord_index + 0],
					1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
				};

				if (uniqueVertices.count(vertex) == 0) {
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
				}

				// Material ID
				//vertex.MaterialID =	shape.mesh.material_ids[attrib.vertices[3 * index.vertex_index + 0]];

				//if (name == "Window")
				//	std::cout << "\nvertex.MaterialID: " << vertex.MaterialID;


				indices.push_back(uniqueVertices[vertex]);

				// THIS MADE A VECTOR SUBSCRIPT OUT OF RANGE ERROR
				//std::cout << "\nmaterialID: " << shape.mesh.material_ids[index.vertex_index];

				//int i = index.

			//	shape.mesh.material_ids[(int)index];
			}

	/*		if (name == "Window")
			for (int i = 0; i < shape.mesh.material_ids.size(); i++)
			{
				std::cout << "\nmaterialID: " << shape.mesh.material_ids[i];
			}*/



			for (int i = 0; i < indices.size(); i += 3) {
				Util::SetNormalsAndTangentsFromVertices(&vertices[indices[i]], &vertices[indices[i + 1]], &vertices[indices[i + 2]]);
			}
			std::cout << "\n indices.size(): " << indices.size();

			Mesh* mesh = new Mesh(vertices, indices, shape.name.c_str());
			this->m_meshes.push_back(mesh);
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
		if (!m_loadedToGL) return;
		if (m_meshes.size() <= 0) return;

		if (m_meshes[meshIndex]->materialID_Set0 != -1)
			AssetManager::BindMaterial_0(m_meshes[meshIndex]->materialID_Set0);
		if (m_meshes[meshIndex]->materialID_Set1 != -1)
			AssetManager::BindMaterial_1(m_meshes[meshIndex]->materialID_Set1);

		shader->setMat4("model", modelMatrix);
		m_meshes[meshIndex]->Draw();
	}



	void Model::Set_Mesh_MaterialID_Set0(const char* meshName, unsigned int materialID)
	{
		for (size_t i = 0; i < m_meshes.size(); i++)
		{
			if (Util::StrCmp(meshName, m_meshes[i]->name))
				m_meshes[i]->materialID_Set0 = materialID;
		}
	}

	void Model::Set_Mesh_MaterialID_Set1(const char* meshName, unsigned int materialID)
	{
		for (size_t i = 0; i < m_meshes.size(); i++)
		{
			if (Util::StrCmp(meshName, m_meshes[i]->name))
				m_meshes[i]->materialID_Set1 = materialID;
		}
	}

	/*void Model::SetMaterial(unsigned int materialID)
	{
		for (size_t i = 0; i < m_meshes.size(); i++)
			m_meshes[i]->materialID = materialID;
	}*/
}