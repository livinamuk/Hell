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

	private:
		btCollisionShape* m_pCollisionShape;
		btDefaultMotionState* m_pMotionState;
		btRigidBody* m_pRigidBody;
		//btCollisionObject* m_collisionObject;
		btPairCachingGhostObject* m_pGhostObject;
		bool m_onGround;
		bool m_hittingWall;
		float m_bottomYOffset;
		float m_bottomRoundedRegionYOffset;
		float m_stepHeight;
		btTransform m_motionTransform;
		std::vector<glm::vec3> m_surfaceHitNormals;
		btVector3 m_previousPosition;

		float m_jumpRechargeTimer;

	

		

		void reposition(btVector3 position);


	public:

		btPairCachingGhostObject* ghostObject_;
		btKinematicCharacterController* bulletController_;

		float m_walkingSpeed = 3;// 1.833f;

		float m_deceleration;
		float m_maxSpeed;
		float m_jumpImpulse;
		float m_jumpRechargeTime;
		glm::vec3 m_manualVelocity; // made public to print in imgui

		glm::vec3 newPosition;

		CharacterController();
		CharacterController(const glm::vec3 spawnPos);
	
		void Update(float deltaTime, Camera* camera);
	//	void Jump();

		void CheckCollisions();

		bool IsOnGround() const;
	};
}