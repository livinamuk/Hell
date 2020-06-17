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
		m_loopCurrentAnimation = true;
	}

	void AnimatedEntity::Update(float deltatime)
	{
		UpdateAnimation(deltatime);
	}

	void AnimatedEntity::Draw(Shader* shader, glm::mat4 modelMatrix)
	{
		if (m_skinnedModelID == -1) return;

		for (unsigned int i = 0; i < m_animatedTransforms.size(); i++)
			shader->setMat4("skinningMats[" + std::to_string(i) + "]", m_animatedTransforms[i]);

		SkinnedModel* skinnedModel = AssetManager::skinnedModels[m_skinnedModelID];

		shader->setInt("hasAnimation", true);
		skinnedModel->Render(shader, modelMatrix * m_worldTransform.to_mat4() * m_modelTransform.to_mat4());

		shader->setInt("hasAnimation", false);
	}

	void AnimatedEntity::SetSkinnedModel(const char* skinnedModelName)
	{
		m_skinnedModelID = AssetManager::GetSkinnedModelIDByName(skinnedModelName);

		int bonecount = AssetManager::skinnedModels[m_skinnedModelID]->m_NumBones;

		m_animatedTransforms.clear();
		m_animatedDebugTransforms_Animated.clear();

		// maybe u can just do below
	//	Transforms.clear();
	//	DebugAnimatedTransforms.clear();

		for (int i  = 0; i < bonecount; i++)
		{
			m_animatedTransforms.push_back(glm::mat4(1));
			m_animatedDebugTransforms_Animated.push_back(glm::mat4(1));
		}
	}

	void AnimatedEntity::PlayAnimation(const char* animationName, bool loop)
	{
		if (m_skinnedModelID == -1) return;

		SkinnedModel* skinnedModel = AssetManager::skinnedModels[m_skinnedModelID];

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
		if (m_skinnedModelID == -1) 
			return glm::mat4(1);

		SkinnedModel* skinnedModel = AssetManager::skinnedModels[m_skinnedModelID];
		return skinnedModel->m_CameraMatrix;
	}

	void AnimatedEntity::UpdateAnimation(float deltatime)
	{
		if (m_skinnedModelID == -1) return;
		SkinnedModel* skinnedModel = AssetManager::skinnedModels[m_skinnedModelID];

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
		skinnedModel->BoneTransform(m_currentAnimationTime, m_animatedTransforms, m_animatedDebugTransforms_Animated);
	}

	void AnimatedEntity::SetAnimationToBindPose()
	{
		if (m_skinnedModelID == -1) return;
		SkinnedModel* skinnedModel = AssetManager::skinnedModels[m_skinnedModelID];
		skinnedModel->BoneTransform(m_currentAnimationTime, m_animatedTransforms, m_animatedDebugTransforms_Animated);
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
	void AnimatedEntity::SetModelScale(float scale)
	{
		m_modelTransform.scale = glm::vec3(scale);
	}

	void AnimatedEntity::FlipModelUpAxis(bool flip) // This includes the skeleton btw
	{
		if (flip)
			m_modelTransform.rotation = glm::vec3(HELL_PI / -2, 0, 0);
		else
			m_modelTransform.rotation = glm::vec3(0);
	}

	void AnimatedEntity::FlipSkeletonUpAxis(bool flip)
	{
		if (flip)
			m_skeletonTransform.rotation = glm::vec3(HELL_PI / 2, 0, 0);
		else
			m_skeletonTransform.rotation = glm::vec3(0);
	}
}