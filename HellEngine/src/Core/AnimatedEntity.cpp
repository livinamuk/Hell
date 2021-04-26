#include "hellpch.h"
#include "AnimatedEntity.h"
#include "Helpers/Util.h"
#include "GL/GpuProfiling.h"

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
		GpuProfiler g("AnimatedEntity.Draw()"); 
		

		for (unsigned int i = 0; i < m_animatedTransforms.size(); i++)
			//shader->setMat4("skinningMats[" + std::to_string(i) + "]", m_animatedTransforms[i]);
			shader->setMat4("skinningMats[" + std::to_string(i) + "]", m_modelTransform.to_mat4() * m_animatedTransforms[i]);

		SkinnedModel* skinnedModel = GetSkinnedModel();

		shader->setInt("hasAnimation", true);
		skinnedModel->currentAnimationIndex = m_currentAnimationIndex; // this fixes the weird bug where if two animated entities have the same skinned model. But it doesn't make any sense because Render() doesn't even make use of this. Try and find out one day, or look here if u have problems later.
		// actually I just found why this MIGHT be the case. skinnedModel has it's own m_currentAnimationIndex which is probably being used somewhere.
		//skinnedModel->Render(shader, modelMatrix * m_worldTransform.to_mat4() * m_modelTransform.to_mat4());
		
		glBindVertexArray(skinnedModel->m_VAO);
		shader->setMat4("model", modelMatrix);

		for (int i = 0; i < skinnedModel->m_meshEntries.size(); i++) {
			AssetManager::BindMaterial_0(m_MaterialIDs[i]);
			glDrawElementsBaseVertex(GL_TRIANGLES, skinnedModel->m_meshEntries[i].NumIndices, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * skinnedModel->m_meshEntries[i].BaseIndex), skinnedModel->m_meshEntries[i].BaseVertex);
		}
		
		shader->setInt("hasAnimation", false);



		
	}

	void AnimatedEntity::SetSkinnedModel(const char* skinnedModelName)
	{
		m_skinnedModelID = AssetManager::GetSkinnedModelIDByName(skinnedModelName);

		SkinnedModel* skinnedModel = AssetManager::skinnedModels[m_skinnedModelID];

		m_animatedTransforms.clear();
		m_animatedDebugTransforms_Animated.clear();

		for (unsigned int i = 0; i < skinnedModel->m_NumBones; i++) {
			m_animatedTransforms.push_back(glm::mat4(1));
			m_animatedDebugTransforms_Animated.push_back(glm::mat4(1));
		}

		// Resize materialIDs to match mesh count
		m_MaterialIDs.resize(skinnedModel->m_meshEntries.size());
	}

	void AnimatedEntity::SetMaterial(const char* MeshName, const char* MaterialName)
	{
		SkinnedModel* skinnedModel = AssetManager::skinnedModels[m_skinnedModelID];

		for (int i = 0; i < skinnedModel->m_meshEntries.size(); i++) {
			if (Util::StrCmp(MeshName, skinnedModel->m_meshEntries[i].Name.c_str()))
				m_MaterialIDs[i] = AssetManager::GetMaterialIDByName(MaterialName);
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

	SkinnedModel* AnimatedEntity::GetSkinnedModel()
	{
		return AssetManager::skinnedModels[m_skinnedModelID];
	}

	void AnimatedEntity::UpdateAnimation(float deltatime)
	{
		//deltatime *= 0.1f;

		if (m_skinnedModelID == -1) return;
		SkinnedModel* skinnedModel = AssetManager::skinnedModels[m_skinnedModelID];

		if (m_currentAnimationIndex == -1 || m_currentAnimationIndex >= skinnedModel->m_animations.size())
			return;

		if (!m_pause)
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
				// Get timestamp of final frame
				// this might be dumb. think about it when youaren't tired.
				// its probably an error with the animation interpotion not here.
				m_currentAnimationTime = m_currentAnimationDuration - 0.01f;// skinnedModel->m_animations[m_currentAnimationIndex]->m_finalTimeStamp / skinnedModel->m_animations[m_currentAnimationIndex]->m_ticksPerSecond;
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

	void AnimatedEntity::PauseAnimation()
	{
		m_pause = true;
	}

	void AnimatedEntity::InitRagdollFromMesh()
	{
	}

	bool AnimatedEntity::IsAnimationComplete()
	{
		return m_animationIsComplete;
	}

	//bool AnimatedEntity::IsSpecificAnimationComplete(const char* animationName)
	//{	
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
	//	return false;
	//}

	void AnimatedEntity::ResetAnimationTimer()
	{
		m_currentAnimationTime = 0;
	}

	void AnimatedEntity::PauseOnFinalFrame()
	{
		//m_pause = true;
		m_currentAnimationTime = m_currentAnimationDuration - 0.001f;
	}
	void AnimatedEntity::SetModelScale(float scale)
	{
		m_modelTransform.scale = glm::vec3(scale);
	}

	/*void AnimatedEntity::FlipModelUpAxis(bool flip) 
	{
		if (flip)
			m_modelTransform.rotation = glm::vec3(HELL_PI / -2, 0, 0);
		else
			m_modelTransform.rotation = glm::vec3(0);
	}*/
}