#include "hellpch.h"
#include "EnemyCharacter.h"
#include "Helpers/Util.h"
#include "GL/GpuProfiling.h"

namespace HellEngine
{
	EnemyCharacter::EnemyCharacter()
	{

	}

	void EnemyCharacter::Draw(Shader* shader, glm::mat4 modelMatrix)
	{
		GpuProfiler g("EnemyCharacter");
		if (m_skinnedModelID == -1) return;

		for (unsigned int i = 0; i < m_animatedTransforms.size(); i++)
			shader->setMat4("skinningMats[" + std::to_string(i) + "]", m_animatedTransforms[i]);

		SkinnedModel* skinnedModel = AssetManager::skinnedModels[m_skinnedModelID];

		shader->setInt("hasAnimation", true);

		//if (m_animatedWithRagdoll)
	//		skinnedModel->Render(shader, m_modelTransform.to_mat4());
	//	else
			skinnedModel->Render(shader, modelMatrix * m_worldTransform.to_mat4() * m_modelTransform.to_mat4());
	
		shader->setInt("hasAnimation", false);
	}

	void EnemyCharacter::NewRagdollFromAnimatedTransforms()
	{
		glm::mat4 worldMatrix = m_worldTransform.to_mat4() * m_modelTransform.to_mat4() * m_skeletonTransform.to_mat4();
		worldMatrix = m_skeletonTransform.to_mat4();
		m_ragdoll = new Ragdoll(btVector3(0, 0, 0), m_modelTransform.scale.x, worldMatrix);

		m_ragdoll->p_modelTransform = &this->m_modelTransform;
		m_ragdoll->p_worldTransform = &this->m_worldTransform;
	}

	void EnemyCharacter::AnimatedFromRagdoll()
	{
		m_ragdoll->UpdateBoneTransform(AssetManager::skinnedModels[m_skinnedModelID], m_animatedTransforms, m_animatedDebugTransforms_Animated);
	}
}