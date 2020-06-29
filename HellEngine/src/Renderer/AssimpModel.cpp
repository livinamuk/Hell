#include "hellpch.h"
#include "Renderer/Mesh.h"
#include "Header.h"
#include "AssimpModel.h"
#include "GL/GpuProfiling.h"
namespace HellEngine
{
	AssimpModel::AssimpModel()
	{
	}

	AssimpModel::AssimpModel(std::string const& path, std::vector<Mesh> meshList)
	{
		this->name = path;
		this->meshes = meshList;
	}

	AssimpModel::~AssimpModel()
	{
	}

	void AssimpModel::Draw(Shader* shader)//, glm::mat4 * model = NULL)
	{
		GpuProfiler g("AssimpModel");
		Transform trans;
		glm::mat4 modelMatrix = trans.to_mat4();
		shader->setMat4("model", modelMatrix);

		// Draw
		for (unsigned int i = 0; i < meshes.size(); i++)
			meshes[i].Draw();

	}
}