#include "hellpch.h"
#include "Renderer/Mesh.h"
#include "Header.h"
#include "AssimpModel.h"

namespace HellEngine
{
	AssimpModel::AssimpModel()
	{
	}

	AssimpModel::AssimpModel(std::string const& path, MeshList meshList)
	{
		this->name = path;
		this->meshes = meshList;
	}

	AssimpModel::~AssimpModel()
	{
	}

	void AssimpModel::Draw(Shader* shader)//, glm::mat4 * model = NULL)
	{
		Transform trans;
		glm::mat4 modelMatrix = trans.to_mat4();
		shader->setMat4("model", modelMatrix);

		// Draw
		for (unsigned int i = 0; i < meshes.size(); i++)
			meshes[i].Draw();

	}
}