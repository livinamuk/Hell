#include "hellpch.h"
#include "CharacterController.h"

namespace HellEngine
{
	CharacterController::CharacterController() {}

	CharacterController::CharacterController(const glm::vec3 spawnPos)
	{
		float radius = 0.25f;
		float height = 0.6f;
		float mass = 5;

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
		bulletController_ = new btKinematicCharacterController(ghostObject_, capsule, 0.3f, btVector3(0, 0, 1));
		bulletController_->setGravity(Physics::s_dynamicsWorld->getGravity());

		int group = CollisionGroups::PLAYER;
		int mask = CollisionGroups::HOUSE | CollisionGroups::ENEMY;

		Physics::s_dynamicsWorld->addCollisionObject(ghostObject_, group, mask);
		Physics::s_dynamicsWorld->addAction(bulletController_);
		bulletController_->setMaxJumpHeight(1.5);
	}
	
	void CharacterController::Update(float deltaTime, Camera* camera)
	{
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

		glm::vec3 vector = targetVelocity * m_walkingSpeed;
		bulletController_->setWalkDirection(Util::glmVec3_to_btVec3(vector));

		btTransform t;
		//t = bulletController_->getGhostObject()->getWorldTransform();
		//bulletController_->getGhostObject()->setWorldTransform(t);
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

	bool CharacterController::IsOnGround() const
	{
		return m_onGround;
	}
}