#pragma once

#include "Physics.h"
#include "Helpers/Util.h"
#include "Core/Camera.h"

namespace HellEngine
{
	class CharacterController
	{
	public:
		glm::vec3 GetWorldPosition();
		glm::mat4 GetWorldMatrix();

		float m_viewHeightStanding = 1.55f;
		float m_viewHeightCrouching = 1.0f;
		float m_crouchDownSpeed = 17.5f;
		float m_currentViewHeight;
		bool m_isCrouching;

	private:
		btCollisionShape* m_pCollisionShape;
		btDefaultMotionState* m_pMotionState;
		btRigidBody* m_pRigidBody;
		btPairCachingGhostObject* m_pGhostObject;
		

		void reposition(btVector3 position);

	public:

		btPairCachingGhostObject* ghostObject_;
		btKinematicCharacterController* bulletController_;

		float radius = 0.25f;
		float height = 0.9f;
		float mass = 5;

		float m_walkingSpeed = 2.5f;

		CharacterController();
		CharacterController(const glm::vec3 spawnPos);
	
		void Update(float deltaTime, Camera* camera);
		glm::vec3 GetViewPosition();
	};
}