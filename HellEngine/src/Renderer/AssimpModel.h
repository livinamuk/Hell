#pragma once
#include "Renderer/Mesh.h"

struct aiNode;
struct aiScene;
struct aiMesh;
struct aiMaterial;
enum aiTextureType;

namespace HellEngine {

	typedef std::vector<Mesh> MeshList;

	class AssimpModel
	{
	public: // methods
		AssimpModel();
		AssimpModel(std::string const& path, MeshList meshList);
		~AssimpModel();
		void Draw(Shader* shader);

	public: // fields
		std::string name = "NO DATA";
		std::vector<Mesh> meshes;
		std::vector<glm::mat4> animatedTransforms;
	};
}