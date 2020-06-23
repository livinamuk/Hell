#include "hellpch.h"
#include "CharacterController.h"

namespace HellEngine
{
	CharacterController::CharacterController() {}

	CharacterController::CharacterController(const glm::vec3 spawnPos)
	{
		btTransform startTransform;
		startTransform.setIdentity();
		startTransform.setOrigin(btVector3(spawnPos.x, spawnPos.y, spawnPos.z));
		startTransform.setRotation(btQuaternion(1, 0, 0, 1));

		btConvexShape* capsule = new btCapsuleShape(radius, height);
		
		ghostObject_ = new btPairCachingGhostObject();
		ghostObject_->setWorldTransform(startTransform);
		Physics::s_dynamicsWorld->getBroadphase()->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
		ghostObject_->setCollisionShape(capsule);
		ghostObject_->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);

		EntityData* entityData = new EntityData();
		entityData->name = "PLAYER";
		entityData->vectorIndex = 0;
		ghostObject_->setUserPointer(entityData);

		//ghostObject_->getCollisionShape()->setMargin(0.04);
		bulletController_ = new btKinematicCharacterController(ghostObject_, capsule, 0.3f);
		bulletController_->setGravity(Physics::s_dynamicsWorld->getGravity());

		int group = CollisionGroups::PLAYER;
		int mask = CollisionGroups::HOUSE | CollisionGroups::ENEMY;

		Physics::s_dynamicsWorld->addCollisionObject(ghostObject_, group, mask);
		Physics::s_dynamicsWorld->addAction(bulletController_);
	}
	
	void CharacterController::Update(float deltaTime, Camera* camera)
	{
		///////////////
		// Crouching //
		///////////////

		if (Input::s_keyDown[HELL_KEY_LEFT_CONTROL])
			m_isCrouching = true;
		else
			m_isCrouching = false;

		float target = m_isCrouching ? m_viewHeightCrouching : m_viewHeightStanding;
		m_currentViewHeight = Util::FInterpTo(m_currentViewHeight, target, deltaTime, m_crouchDownSpeed);

		btVector3 capsuleScaling = m_isCrouching ? btVector3(1, 0.5f, 1) : btVector3(1, 1, 1);;
		ghostObject_->getCollisionShape()->setLocalScaling(capsuleScaling);

		/////////////
		// Jumping //
		/////////////
		
		if (Input::s_keyPressed[HELL_KEY_SPACE] && bulletController_->canJump())
		{
			bulletController_->setJumpSpeed(m_jumpStrength);
			bulletController_->jump(); 
		}

		//////////////
		// Movement //
		//////////////

		glm::vec3 targetVelocity = glm::vec3(0, 0, 0);
		glm::vec3 Forward = glm::normalize(glm::vec3(camera->m_Front.x, 0, camera->m_Front.z));

		if (Input::s_keyDown[HELL_KEY_W]) {
			targetVelocity += Forward * deltaTime;
		}
		if (Input::s_keyDown[HELL_KEY_S] ) {
			targetVelocity -= Forward * deltaTime;
		}
		if (Input::s_keyDown[HELL_KEY_A]) {
			targetVelocity -= camera->m_Right * deltaTime;
		}
		if (Input::s_keyDown[HELL_KEY_D]) {
			targetVelocity += camera->m_Right * deltaTime;
		}

		if (Input::s_keyDown[HELL_KEY_LEFT_SHIFT] || Input::s_keyDown[HELL_KEY_RIGHT_SHIFT])
			m_currentMovementSpeed = m_movementSpeedRunning;		
		else if (m_isCrouching)
			m_currentMovementSpeed = m_movementSpeedCrouching;
		else
			m_currentMovementSpeed = m_movementSpeedWalking;

		glm::vec3 vector = targetVelocity * m_currentMovementSpeed;
		bulletController_->setWalkDirection(Util::glmVec3_to_btVec3(vector));
	}

	void CharacterController::reposition(btVector3 position) 
	{
		btTransform initialTransform;

		initialTransform.setOrigin(position);

		m_pRigidBody->getWorldTransform().setOrigin(position);
		m_pGhostObject->getWorldTransform().setOrigin(position);
		m_pMotionState->setWorldTransform(initialTransform);
	}

	glm::vec3 CharacterController::GetWorldPosition()
	{
		float x = bulletController_->getGhostObject()->getWorldTransform().getOrigin().getX();
		float y = bulletController_->getGhostObject()->getWorldTransform().getOrigin().getY();
		float z = bulletController_->getGhostObject()->getWorldTransform().getOrigin().getZ();
		return glm::vec3(x, y, z);
	}

	glm::mat4 CharacterController::GetWorldMatrix()
	{
		Transform trans;
		trans.position = GetWorldPosition();
		return trans.to_mat4();
	}

	/*bool CharacterController::IsOnGround() const
	{
		return m_onGround;
	}*/

	glm::vec3 CharacterController::GetViewPosition()
	{
		glm::vec3 worldPos = Util::btVec3_to_glmVec3(bulletController_->getGhostObject()->getWorldTransform().getOrigin());
		float capsuleScaling = ghostObject_->getCollisionShape()->getLocalScaling().getY();
		float bottomOfCapsuleYPosition = worldPos.y - (height * capsuleScaling * 0.5f) - radius;
		worldPos.y = bottomOfCapsuleYPosition + m_currentViewHeight;
		return worldPos;
	}
}