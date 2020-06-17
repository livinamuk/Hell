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
		glm::mat4 worldMatrix = m_worldTransform.to_mat4() * m_modelTransform.to_mat4() * m_skeletonTransform.to_mat4();
		worldMatrix = m_skeletonTransform.to_mat4();
		m_ragdoll = new Ragdoll(btVector3(0, 0, 0), m_modelTransform.scale.x, worldMatrix);
	}
}