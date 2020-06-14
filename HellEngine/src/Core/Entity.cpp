#include "hellpch.h"
#include "Entity.h"
#include "Helpers/Util.h"
#include "Helpers/AssetManager.h"

namespace HellEngine
{
	Entity::Entity()
	{
	}

	Entity::Entity(char* tag)
	{
		m_tag = tag;
	}

	Entity::Entity(int modelID)
	{
		m_modelID = modelID;
	}

	void Entity::Update(float deltaTime)
	{

	}

	void Entity::DrawEntity(Shader* shader)
	{
		// Bind material
		//if (this->m_materialID != -1 && this->m_materialID < AssetManager::materials.size())
			AssetManager::BindMaterial(m_materialID);

	//	std::cout << AssetManager::currentlyBoundMaterialID << "\n";

			AssetManager::models[m_modelID].Draw(shader, m_transform.to_mat4());

		// Draw model
		//if (this->m_modelID != -1 && this->m_modelID < AssetManager::models.size())
		//	AssetManager::DrawModel(this->m_modelID, shader, m_transform.to_mat4());
	}

	void Entity::Draw(Shader* shader, glm::mat4 modelMatrix)
	{
		//if (m_modelID != -1)
		//	AssetManager::models[m_modelID].CalculateAnimation(m_currentAnimationName, m_currentAnimationTime);
		
		AssetManager::DrawModel(m_modelID, shader, modelMatrix);
	}

	void Entity::Draw(Shader* shader)
	{
		//if (m_modelID != -1)
		//	AssetManager::models[m_modelID].CalculateAnimation(m_currentAnimationName, m_currentAnimationTime);
		
		AssetManager::DrawModel(m_modelID, shader, m_transform.to_mat4());
	}
}