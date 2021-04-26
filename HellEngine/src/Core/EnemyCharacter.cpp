#include "hellpch.h"
#include "EnemyCharacter.h"
#include "Helpers/Util.h"

namespace HellEngine
{
	EnemyCharacter::EnemyCharacter()
	{

	}

	

	void EnemyCharacter::NewRagdollFromAnimatedTransforms()
	{
		//glm::mat4 worldMatrix = m_worldTransform.to_mat4() * m_modelTransform.to_mat4() * m_skeletonTransform.to_mat4();
		//worldMatrix = m_skeletonTransform.to_mat4();
		glm::mat4 worldMatrix = m_worldTransform.to_mat4();
		m_ragdoll = new Ragdoll2("ragdoll.json");


		/* OLD
		m_ragdoll = new Ragdoll(btVector3(0, 0, 0), m_modelTransform.scale.x, worldMatrix);
		m_ragdoll->p_modelTransform = &this->m_modelTransform;
		m_ragdoll->p_worldTransform = &this->m_worldTransform;
		*/
	}

	void EnemyCharacter::AnimatedFromRagdoll()
	{
		m_ragdoll->UpdateBoneTransform(GetSkinnedModel(), m_animatedTransforms, m_animatedDebugTransforms_Animated);
	}
}