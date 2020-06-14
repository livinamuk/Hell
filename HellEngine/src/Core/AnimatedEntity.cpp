#include "hellpch.h"
#include "AnimatedEntity.h"
#include "Helpers/Util.h"

namespace HellEngine
{
	AnimatedEntity::AnimatedEntity()
	{
		m_currentAnimationIndex = -1;
		m_currentAnimationTime = 0;
		m_animationSpeed = 1;
	}

	void AnimatedEntity::Update(float deltatime)
	{
		UpdateAnimation(deltatime);
	}

	void AnimatedEntity::Draw(Shader* shader, glm::mat4 modelMatrix)
	{
		int ID = AssetManager::GetSkinnedModelIDByName("Shotgun.fbx");
		SkinnedModel* skinnedModel = AssetManager::skinnedModels[ID];
		skinnedModel->Render(shader, modelMatrix);
	}

	void AnimatedEntity::PlayAnimation(const char* animationName, bool loop)
	{
		int ID = AssetManager::GetSkinnedModelIDByName("Shotgun.fbx");
		SkinnedModel* skinnedModel = AssetManager::skinnedModels[ID];

		// Set animation index
		for (size_t i = 0; i < skinnedModel->m_animations.size(); i++)
		{
			if (std::strcmp(skinnedModel->m_animations[i]->m_filename, animationName) == 0)
			{
				// are you already playing this?
				if (m_currentAnimationIndex == i)
					return;

				// otherwise play it
				m_currentAnimationIndex = i;
				m_currentAnimationTime = 0;
				m_loopCurrentAnimation = loop;
				m_animationIsComplete = false;
				return;
			}
		}
	}

	glm::mat4 AnimatedEntity::GetCameraMatrix()
	{
		int ID = AssetManager::GetSkinnedModelIDByName("Shotgun.fbx");
		SkinnedModel* skinnedModel = AssetManager::skinnedModels[ID];
		return skinnedModel->m_CameraMatrix;
	}

	void AnimatedEntity::UpdateAnimation(float deltatime)
	{
		int ID = AssetManager::GetSkinnedModelIDByName("Shotgun.fbx");
		SkinnedModel* skinnedModel = AssetManager::skinnedModels[ID];

		if (m_currentAnimationIndex == -1 || m_currentAnimationIndex >= skinnedModel->m_animations.size())
			return;

		m_currentAnimationTime += deltatime * m_animationSpeed;		
		m_currentAnimationDuration = skinnedModel->m_animations[m_currentAnimationIndex]->m_duration / 30.0f;

		// Looping
		m_animationIsComplete = false;
		if (m_loopCurrentAnimation)
			if (m_currentAnimationTime > m_currentAnimationDuration)
				m_currentAnimationTime = 0;
		// No loop
		if (!m_loopCurrentAnimation)
			if (m_currentAnimationTime >= m_currentAnimationDuration)
			{
				m_currentAnimationTime = m_currentAnimationDuration - 0.001f; 
				m_animationIsComplete = true;
			}

		skinnedModel->currentAnimationIndex = m_currentAnimationIndex;
		skinnedModel->BoneTransform(m_currentAnimationTime, m_animatedTransforms);
	}

	bool AnimatedEntity::IsAnimationComplete()
	{
		return m_animationIsComplete;
	}

	bool AnimatedEntity::IsSpecificAnimationComplete(const char* animationName)
	{	
		// be wary. this check runs even if the desired aniamation aint playing. consider merging with the above to awlauys check the current.

		/*int ID = AssetManager::GetSkinnedModelIDByName("Shotgun.fbx");
		SkinnedModel* skinnedModel = AssetManager::skinnedModels[ID];

		for (size_t i = 0; i < skinnedModel->m_animations.size(); i++)
		{
			if (std::strcmp(skinnedModel->m_animations[i]->m_filename, animationName) == 0)
			{
				float duration = skinnedModel->m_animations[i]->m_duration / 30.0f;
				
				if (m_currentAnimationTime >= duration)
				{
					m_currentAnimationTime = m_currentAnimationDuration - 0.001f;
					m_animationIsComplete = true;
			}
		}
		*/
		return false;
	}

	void AnimatedEntity::ResetAnimationTimer()
	{
		m_currentAnimationTime = 0;
	}

	void AnimatedEntity::PauseOnFinalFrame()
	{
		m_loopCurrentAnimation = false;
		m_currentAnimationTime = m_currentAnimationDuration - 0.001f;
	}
}