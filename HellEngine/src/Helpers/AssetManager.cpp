#include "hellpch.h"
#include "AssetManager.h"
#include "Util.h"
#include "Game.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/matrix4x4.h"

namespace HellEngine
{
	std::vector<Texture> AssetManager::textures;
	std::vector<Material> AssetManager::materials;
	std::vector<Model> AssetManager::models;
	std::vector<SkinnedModel*> AssetManager::skinnedModels;

	std::mutex AssetManager::s_TexturesMutex;
	std::mutex AssetManager::s_ModelsMutex;
	std::mutex AssetManager::s_AnimationsMutex;
	std::vector<std::future<void>> AssetManager::m_Futures;
	unsigned int AssetManager::currentlyBoundMaterialID;

	unsigned int AssetManager::s_MaterialID_FloorBoards;
	unsigned int AssetManager::s_MaterialID_PlasterCeiling;
	unsigned int AssetManager::s_MaterialID_WallPaper;
	unsigned int AssetManager::s_MaterialID_Trims;

	unsigned int AssetManager::s_ModelID_CeilingTrim;
	unsigned int AssetManager::s_ModelID_FloorTrim;
	unsigned int AssetManager::s_ModelID_DoorFrame;
	unsigned int AssetManager::s_ModelID_Door;	
	
	unsigned int AssetManager::s_ModelID_StaircaseCeilingTrimStraight;
	unsigned int AssetManager::s_ModelID_Staircase;

	bool AssetManager::s_loadingComplete = false;
	std::string AssetManager::s_loadLog = "WELCOME TO HELL\nLoadng please wait...\n";

	void AssetManager::FindAllFiles()
	{
		// Find all textures
		std::string path = "res/textures/";
		for (const auto& entry : std::filesystem::directory_iterator(path))
		{
			std::stringstream ss;
			ss << entry.path();
			std::string filename = ss.str();

			filename = Util::RemoveCharactersFromEnd(filename, 1);
			filename = Util::RemoveCharactersFromBeginning(filename, path.length() + 1);
			std::string filetype = Util::RemoveCharactersFromBeginning(filename, filename.length() - 3);
			filename = Util::RemoveCharactersFromEnd(filename, 4);

			if ((filetype != "tga") && (filetype != "png") && (filetype != "jpg"))
				continue;

			textures.push_back(Texture(filename, filetype));
		}
	}
	void AssetManager::ForceLoadTexture(std::string name)
	{
		for (Texture& texture : textures)
		{
			if (texture.name == name)
			{
				texture.ReadFromDisk();
				texture.LoadToGL();
			}
		}
	}

	void AssetManager::LoadAnimation(const char* skinnedModelName, const char* filename)
	{
		for (SkinnedModel* skinnedModel : skinnedModels)
		{
			if (std::strcmp(skinnedModel->m_filename, skinnedModelName) == 0)
			{
				skinnedModel->LoadAnimation(filename);
				return;
			}
		}
		std::cout << "Couldn't add animation [" << filename << "] because couldn't find [" << skinnedModelName << "] :(\n";
	}

	/*int AssetManager::GetTextureWidth(int textureID)
	{
		if (textureID < 0 || textureID >= textures.size())
			return -1;
		else
			return textures[textureID].width;
	}

	int AssetManager::GetTextureHeight(int textureID)
	{
		if (textureID < 0 || textureID >= textures.size())
			return -1;
		else
			return textures[textureID].height;
	}*/

	void AssetManager::LoadAllTextures()
	{
		// Multi-thead load them
		for (Texture& texture : textures)			
			m_Futures.push_back(std::async(std::launch::async, MultiThreadedReadTextureFromDisk, &texture));
	}

	void AssetManager::LoadHardcoded()
	{
		models.emplace_back(Model("res/models/StaircaseCeilingTrimStraight.obj"));
		models.emplace_back(Model("res/models/TrimFloor.obj"));
		models.emplace_back(Model("res/models/TrimCeiling.obj"));
		models.emplace_back(Model("res/models/Door.obj"));
		models.emplace_back(Model("res/models/DoorVolumeA.obj"));
		models.emplace_back(Model("res/models/DoorShadowCaster.obj"));
		models.emplace_back(Model("res/models/DoorFrame.obj"));
		models.emplace_back(Model("res/models/Staircase.obj"));
		models.emplace_back(Model("res/models/Light.obj"));
		models.emplace_back(Model("res/models/sphere.obj"));
		models.emplace_back(Model("res/models/PictureFrame2.obj"));
		models.emplace_back(Model("res/models/PictureFrame.obj"));
		models.emplace_back(Model("res/models/Couch.obj"));
		models.emplace_back(Model("res/models/Mannequin.obj"));
		models.emplace_back(Model("res/models/Shell.obj"));

		skinnedModels.emplace_back(new SkinnedModel("Zombie.fbx"));

		skinnedModels.emplace_back(new SkinnedModel("Shotgun.fbx"));
		LoadAnimation("Shotgun.fbx", "Shotgun_Idle.fbx");
		LoadAnimation("Shotgun.fbx", "Shotgun_Walk.fbx");
		LoadAnimation("Shotgun.fbx", "Shotgun_Fire.fbx");
		LoadAnimation("Shotgun.fbx", "Shotgun_ReloadWetStart.fbx");
		LoadAnimation("Shotgun.fbx", "Shotgun_Reload1Shell.fbx");
		LoadAnimation("Shotgun.fbx", "Shotgun_Reload2Shells.fbx");
		LoadAnimation("Shotgun.fbx", "Shotgun_ReloadEnd.fbx");
		LoadAnimation("Shotgun.fbx", "Shotgun_ReloadDryStart.fbx");
		LoadAnimation("Shotgun.fbx", "Shotgun_Equip.fbx");
		LoadAnimation("Shotgun.fbx", "Shotgun_Dequip.fbx");
		LoadAnimation("Shotgun.fbx", "Shotgun_GunUp.fbx");
		LoadAnimation("Shotgun.fbx", "Shotgun_GunUpWalk.fbx");
		LoadAnimation("Shotgun.fbx", "Shotgun_GunDown.fbx");
		LoadAnimation("Shotgun.fbx", "Shotgun_SprintStart.fbx");
		LoadAnimation("Shotgun.fbx", "Shotgun_SprintLoop.fbx");
		LoadAnimation("Shotgun.fbx", "Shotgun_SprintStop.fbx");
		LoadAnimation("Shotgun.fbx", "Shotgun_SightingIn.fbx");
		LoadAnimation("Shotgun.fbx", "Shotgun_SightingWalk.fbx");
		LoadAnimation("Shotgun.fbx", "Shotgun_SightingFire.fbx");
		LoadAnimation("Shotgun.fbx", "Shotgun_SightingOut.fbx");
		
		for (int i = 0; i < models.size(); i++)
			models[i].ReadFromDisk(); 	

	}

	void AssetManager::MultiThreadedReadTextureFromDisk(Texture* texture)
	{
		std::lock_guard<std::mutex> lock(s_TexturesMutex); 
		texture->ReadFromDisk();
	}

	void AssetManager::AddMaterial(std::string name)
	{
		// Check if material exists already
		bool found = false;
		for (Material& material : materials)
		{
			// If found, update with new texture
			if (material.name == name) {
				found = true;
				material.FindTextures();
				return;
			}
		}
		// Otherwise create it
		materials.emplace_back(Material(name));
		//std::cout << "Created material: " << name << "\n";
	}

	void AssetManager::LoadNextReadyAssetToGL()
	{
		
		// Load in textures
		for (Texture& texture : textures)
		{
			if (texture.m_readFromDisk && !texture.m_loadedToGL)
			{
				texture.LoadToGL();
				s_loadLog += "loaded " + texture.name + "." + texture.filetype + "\n";

				if ((Util::StringEndsIn(texture.name, "_ALB")) ||
					(Util::StringEndsIn(texture.name, "_NRM")) ||
					(Util::StringEndsIn(texture.name, "_RMA")) ||
					(Util::StringEndsIn(texture.name, "_RME")))	{
					AddMaterial(Util::RemoveCharactersFromEnd(texture.name, 4));
				}				

				AssetManager:AssignHardcodedModelMaterials();
				return;
			}
		}
		// Load in mesh data
		for (Model& model : models)
		{
			if (model.m_readFromDisk && !model.m_loadedToGL) {

				s_loadLog += "loaded " + model.name + "." + Util::FileTypeToString(model.m_fileType) + "\n";
				model.LoadMeshDataToGL();
				return;
			}
		}

		// Have you finished loading all your assets? Probably not.
		s_loadingComplete = true;
		for (Model& model : models)
			if (!model.m_loadedToGL)
				s_loadingComplete = false;
		
		for (Texture& texture : textures)
			if (!texture.m_loadedToGL)
				s_loadingComplete = false;

		if (s_loadingComplete)
			TextBlitter::ResetBlitter();
	}

	void AssetManager::AssignHardcodedModelMaterials()
	{
		//SetModelMaterialIDByModelID(GetModelIDByName("Door"), GetMaterialIDByName("Door"));
		//SetModelMaterialIDByModelID(GetModelIDByName("DoorFrame"), GetMaterialIDByName("DoorFrame"));
		//SetModelMaterialIDByModelID(GetModelIDByName("sphere"), GetMaterialIDByName("Red"));

		AssetManager::s_MaterialID_FloorBoards = GetMaterialIDByName("FloorBoards");
		AssetManager::s_MaterialID_PlasterCeiling = GetMaterialIDByName("PlasterCeiling");
		AssetManager::s_MaterialID_WallPaper = GetMaterialIDByName("WallPaper");

		//SetModelMaterialIDByModelID(GetModelIDByName("sphere"), GetMaterialIDByName("Red"));
		//SetModelMaterialIDByModelID(GetModelIDByName("TrimFloor"), GetMaterialIDByName("Trims"));
		//SetModelMaterialIDByModelID(GetModelIDByName("TrimCeiling"), GetMaterialIDByName("Trims"));

		AssetManager::s_ModelID_CeilingTrim = AssetManager::GetModelIDByName("TrimCeiling");
		AssetManager::s_ModelID_FloorTrim = AssetManager::GetModelIDByName("TrimFloor");
		AssetManager::s_ModelID_DoorFrame = AssetManager::GetModelIDByName("DoorFrame");
		AssetManager::s_ModelID_Door = AssetManager::GetModelIDByName("Door");

		AssetManager::s_ModelID_StaircaseCeilingTrimStraight = AssetManager::GetModelIDByName("StaircaseCeilingTrimStraight");
		AssetManager::s_ModelID_Staircase = AssetManager::GetModelIDByName("Staircase");

		//SetModelMaterialIDByModelID(s_ModelID_Staircase, GetMaterialIDByName("Stairs01"));
		//SetModelMaterialIDByModelID(s_ModelID_StaircaseCeilingTrimStraight, GetMaterialIDByName("Trims"));	
	}

	/*void AssetManager::SetModelMaterialIDByModelID(unsigned int modelID, unsigned int materialID)
	{
		if (modelID < 0 || modelID >= models.size())
			return;

		for (Mesh* mesh : models[modelID].m_meshes)
			mesh->materialID = materialID;
		return;
	}*/
	/*
	void AssetManager::SetModelMaterialIDByModelIDMeshName(unsigned int modelID, std::string meshName, unsigned int materialID)
	{
		if (modelID < 0 || modelID >= models.size())
			return;

		for (Mesh* mesh : models[modelID].m_meshes) {
			if (mesh->name == meshName) {
				mesh->materialID = materialID;
				return;
			}
		}
	}*/

	unsigned int AssetManager::GetTexIDByName(std::string textureName)
	{
		for (Texture& texture : textures)
		{
			if (texture.name == textureName)
				return texture.ID;
		}
		//std::cout << textureName + " not found\n";
		return 0;
	}

	Texture* AssetManager::GetTextureByName(char* name)
	{
		for (Texture& texture : textures)
		{
			if (strcmp(texture.name.c_str(), name) == 0)
				return &texture;
		}
		return nullptr;
	}

	int AssetManager::GetModelIDByName(std::string modelName)
	{
		for (size_t i = 0; i < models.size(); i++)
		{
			if (models[i].name == modelName)
				return i;
		}
		return -1;
	}

	int AssetManager::GetSkinnedModelIDByName(const char* name)
	{
		for (size_t i = 0; i < skinnedModels.size(); i++)
		{
			//std::cout << name << " x " << skinnedModels[i]->m_filename << "\n";

			if (std::strcmp(skinnedModels[i]->m_filename, name) == 0)
				return i;
		}
		return -1;
	}

	Model* AssetManager::GetModelByID(int modelID)
	{
		if (modelID < 0 || modelID >= models.size())
			return NULL;
		else
			return &models[modelID];
	}

	Model* AssetManager::GetModelByName(std::string modelName)
	{
		for (size_t i = 0; i < models.size(); i++)
		{
			if (models[i].name == modelName)
				return &models[i];
		}
		std::cout << "GetModelByName(): " << modelName << " not found\n";
		return nullptr;
	}

	std::string AssetManager::GetModelNameByID(int modelID)
	{
		if (modelID < 0 || modelID >= models.size())
			return "No Model";

		return models[modelID].name;
	}

	void AssetManager::DrawModel(int modelID, Shader* shader, glm::mat4 modelMatrix)
	{
		if (modelID == -1) 
			return;
		
		//if (currentlyBoundMaterialID > 0 && currentlyBoundMaterialID < materials.size())
			//if (materials[currentlyBoundMaterialID].RME != 0)
			//	shader->setBool("hasEmissive", true);

		models[modelID].Draw(shader, modelMatrix);

		//shader->setBool("hasEmissive", false);
	}

	void AssetManager::DrawMesh(int modelID, int meshIndex, Shader* shader, glm::mat4 modelMatrix)
	{
		if (modelID == -1)
			return;
		else
			models[modelID].DrawMesh(shader, meshIndex, modelMatrix);
	}

	std::string AssetManager::GetMaterialNameByID(unsigned int materialID)
	{
		if (materialID < 0 || materialID >= materials.size())
			return "OUT OF RANGE MATERIAL ID";
		else
			return materials[materialID].name;
	}

	void AssetManager::PrintSkinnedModelMeshNames(const char* skinnedModelName)
	{
		int ID = GetSkinnedModelIDByName(skinnedModelName);
		
		if (ID == -1) 
			return;

		std::cout << skinnedModelName << " MESH NAMES:\n";

			for (int i = 0; i < skinnedModels[ID]->m_meshEntries.size(); i++)
			std::cout << " - " << skinnedModels[ID]->m_meshEntries[i].MeshName << "\n";
	}

	void AssetManager::PrintSkinnedModelBoneNames(const char* skinnedModelName)
	{
		int ID = GetSkinnedModelIDByName(skinnedModelName);

		if (ID == -1)
			return;

		std::cout << skinnedModelName << " BONE NAMES\n";

		for (int i = 0; i < skinnedModels[ID]->m_BoneInfo.size(); i++)
			std::cout << " " << i << ": " << skinnedModels[ID]->m_BoneInfo[i].BoneName << "\n";
	}

	void AssetManager::BindMaterial(unsigned int materialID)
	{
		if (materialID < 0) return;
		if (materialID >= materials.size()) return;

		Material* material = &materials[materialID];
		currentlyBoundMaterialID = materialID;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, AssetManager::GetAlbTexID(materialID));
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, AssetManager::GetNrmTexID(materialID));
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, AssetManager::GetRmaTexID(materialID));
		glActiveTexture(GL_TEXTURE3);
	}

	unsigned int AssetManager::GetAlbTexID(unsigned int materialID)
	{
		if (materials.size() == 0)
			return 0;
		else return materials[materialID].ALB;
	}

	unsigned int AssetManager::GetRmaTexID(unsigned int materialID)
	{
		if (materials.size() == 0)
			return 0;
		else return materials[materialID].RMA;
	}

	unsigned int AssetManager::GetRmeTexID(unsigned int materialID)
	{
		if (materials.size() == 0)
			return 0;
		else return materials[materialID].RME;
	}

	unsigned int AssetManager::GetNrmTexID(unsigned int materialID)
	{
		if (materials.size() == 0)
			return 0;
		else return materials[materialID].NRM;
	}

	unsigned int AssetManager::GetMaterialIDByName(std::string name)
	{
		for (int i = 0; i < materials.size(); i++)
		{
			if (materials[i].name == name)
				return i;
		}
		return 0;
	}
	/*
	AssimpModel AssetManager::LoadFromFile(std:: string const& path)
	{
		//std::cout << ("LOADING " + path) << "\n";

		// read file via ASSIMP
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile("res/models/" + path,
			aiProcess_Triangulate |
			aiProcess_FlipUVs |
			aiProcess_CalcTangentSpace |
			aiProcess_LimitBoneWeights);

		// check for errors
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
		{
			std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
			return AssimpModel();
		}
		// retrieve the directory path of the filepath
		//currrentDirectory = path.substr(0, path.find_last_of('/'));

		MeshList meshList;

		// Get model data
		processNode(scene->mRootNode, scene, &meshList);
		return AssimpModel(path, meshList);
	}

	void AssetManager::processNode(aiNode* node, const aiScene* scene, MeshList* meshList)
	{
		// process each mesh located at the current node
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			// the node object only contains indices to index the actual objects in the scene. 
			// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshList->push_back(processMesh(mesh, scene));
		}

		// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
		for (unsigned int i = 0; i < node->mNumChildren; i++)
			processNode(node->mChildren[i], scene, meshList);
	}

	Mesh AssetManager::processMesh(aiMesh* mesh, const aiScene* scene)
	{
		// data to fill
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;

		glm::vec3 lowestPositionValues = glm::vec3(10000);
		glm::vec3 highestPositionValues = glm::vec3(-10000);

		// Walk through each of the mesh's vertices
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;

			glm::vec3 vector;
			// positions
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;

			vertex.Position = vector;
			// normals
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
			// texture coordinates
			if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
			{
				glm::vec2 vec;
				// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
				// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoords = vec;

				// tangent
				vector.x = mesh->mTangents[i].x;
				vector.y = mesh->mTangents[i].y;
				vector.z = mesh->mTangents[i].z;
				vertex.Tangent = vector;
				// bitangent
				vector.x = mesh->mBitangents[i].x;
				vector.y = mesh->mBitangents[i].y;
				vector.z = mesh->mBitangents[i].z;
			}
			else
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);

			vertex.Bitangent = vector;
			vertices.push_back(vertex);

			// Store bounding box data
			lowestPositionValues.x = std::min(lowestPositionValues.x, vertex.Position.x);
			lowestPositionValues.y = std::min(lowestPositionValues.y, vertex.Position.y);
			lowestPositionValues.z = std::min(lowestPositionValues.z, vertex.Position.z);
			highestPositionValues.x = std::max(highestPositionValues.x, vertex.Position.x);
			highestPositionValues.y = std::max(highestPositionValues.y, vertex.Position.y);
			highestPositionValues.z = std::max(highestPositionValues.z, vertex.Position.z);
		}
		// walk through each of the mesh's faces and retrieve the corresponding vertex indices.
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			// retrieve all indices of the face and store them in the indices vector
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}

		// Build bounding box
		//BoundingBox boundingBox = BoundingBox(lowestPositionValues, highestPositionValues);


		return Mesh(vertices, indices, mesh->mName.C_Str());
	}*/
}