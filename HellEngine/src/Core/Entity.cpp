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
		// Animation
		fbxsdk::FbxTime time;
		time.SetMilliSeconds((fbxsdk::FbxLongLong)(deltaTime * 1000));
		m_currentAnimationTime += time;

		if (m_currentAnimation == NULL)
			return;

		if (IsAnimationComplete())
		{
			if (m_loopAnimation)
				m_currentAnimationTime = m_currentAnimation->m_startTime;
			else
				m_currentAnimationTime = m_currentAnimation->m_endTime;
		}
	}

	void Entity::SetAnimation(char* animationName, bool loop)
	{
		// Bail if not a valid model
		if (m_modelID == -1)
			return;
		// Bail if already playing that animation
		if (strcmp(m_currentAnimationName, animationName) == 0)
			return;
		// Bail if the animation doesn't exist
		if (AssetManager::models[m_modelID].m_animations.find(animationName) == AssetManager::models[m_modelID].m_animations.end())
			return;
		// Otherwise, reset the timer and switch animation
		m_currentAnimation = &AssetManager::models[m_modelID].m_animations[animationName];
		m_currentAnimationTime = m_currentAnimation->m_startTime;
		m_currentAnimationName = animationName;
		m_loopAnimation = loop;
	}

	bool Entity::IsAnimationComplete()
	{
		if (m_currentAnimation == NULL)
			return false;

		return (m_currentAnimationTime >= m_currentAnimation->m_endTime);
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
		if (m_modelID != -1)
			AssetManager::models[m_modelID].CalculateAnimation(m_currentAnimationName, m_currentAnimationTime);
		
		AssetManager::DrawModel(m_modelID, shader, modelMatrix);
	}

	void Entity::Draw(Shader* shader)
	{
		if (m_modelID != -1)
			AssetManager::models[m_modelID].CalculateAnimation(m_currentAnimationName, m_currentAnimationTime);
		
		AssetManager::DrawModel(m_modelID, shader, m_transform.to_mat4());
	}
}