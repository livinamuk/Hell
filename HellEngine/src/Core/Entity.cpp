#include "hellpch.h"
#include "Entity.h"
#include "Helpers/Util.h"
#include "Helpers/AssetManager.h"
#include "Core/LevelEditor.h"

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
		LevelEditor::SetHighlightColorIfSelected(shader, this);
		AssetManager::BindMaterial(m_materialID);
		AssetManager::models[m_modelID].Draw(shader, m_transform.to_mat4());
		shader->setVec3("ColorAdd", glm::vec3(0, 0, 0));
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