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
		float characterStepHeight = 0.051f;

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
			targetVelocity += camera->m_Right * deltaTime;
		}
		if (Input::s_keyDown[HELL_KEY_D]) {
			targetVelocity -= camera->m_Right * deltaTime;
		}

		glm::vec3 vector = targetVelocity * m_walkingSpeed;
		bulletController_->setWalkDirection(Util::glmVec3_to_btVec3(vector));

		btTransform t;
		t = bulletController_->getGhostObject()->getWorldTransform();
		bulletController_->getGhostObject()->setWorldTransform(t);
	}

	void CharacterController::reposition(btVector3 position) 
	{
		btTransform initialTransform;

		initialTransform.setOrigin(position);

		m_pRigidBody->getWorldTransform().setOrigin(position);
		m_pGhostObject->getWorldTransform().setOrigin(position);
		m_pMotionState->setWorldTransform(initialTransform);
	}

	struct btDrawingResult : public btCollisionWorld::ContactResultCallback
	{
		virtual    btScalar    addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1)
		{
			std::cout << "\n COLLISION FOUND";
			return 0;
		}
	};

	void CharacterController::CheckCollisions()
	{
	//	std::cout << "\n";

		reposition(btVector3(newPosition.x, 0.6f, newPosition.z));

		btManifoldArray manifoldArray; btBroadphasePairArray& pairArray =
			m_pGhostObject->getOverlappingPairCache()->getOverlappingPairArray(); int numPairs =
			pairArray.size();

		for (int i = 0; i < numPairs; ++i) {
			manifoldArray.clear();

			const btBroadphasePair& pair = pairArray[i];

			btBroadphasePair* collisionPair = Physics::s_dynamicsWorld->getPairCache()->findPair(
				pair.m_pProxy0, pair.m_pProxy1);

			if (!collisionPair) continue;

			if (collisionPair->m_algorithm)
				collisionPair->m_algorithm->getAllContactManifolds(manifoldArray);

			Physics::s_points.clear();

			for (int j = 0; j < manifoldArray.size(); j++) {
				btPersistentManifold* manifold = manifoldArray[j];

				bool isFirstBody = manifold->getBody0() == m_pRigidBody;

				btScalar direction = isFirstBody ? btScalar(-1.0) : btScalar(1.0);

				for (int p = 0; p < manifold->getNumContacts(); ++p) {
					const btManifoldPoint& pt = manifold->getContactPoint(p);

					if (pt.getDistance() < 0.f) {
						const btVector3& ptA = pt.getPositionWorldOnA(); const
							btVector3& ptB = pt.getPositionWorldOnB(); const btVector3& normalOnB =
							pt.m_normalWorldOnB;


						auto body = manifold->getBody1();
						
						EntityData* entityData = (EntityData*)body->getUserPointer();
						if (entityData) {
//							std::cout << "name: " << entityData->name << "\n";
						}


						glm::vec3 posB = Util::btVec3_to_glmVec3(pt.getPositionWorldOnB());
						Physics::s_points.push_back(posB);
						glm::vec3 posA = Util::btVec3_to_glmVec3(pt.getPositionWorldOnA());
						Physics::s_points.push_back(posA);

						glm::vec3 normal = posB - posA;
						normal = glm::normalize(normal);


						//btVector3 local = pt.m_localPointB - pt.m_localPointA;
						btVector3 local = pt.getPositionWorldOnB() - pt.getPositionWorldOnA();
						float length = local.length();
						
						
						//glm::vec3 response = normal * 0.1f;
						glm::vec3 response = normal * length;

						//Physics::m_dynamicsWorld->setForceUpdateAllAabbs(true);

						//Physics::m_dynamicsWorld->updateSingleAabb(m_pRigidBody);

						float x = m_pRigidBody->getWorldTransform().getOrigin().getX();
						float z = m_pRigidBody->getWorldTransform().getOrigin().getZ();

						btVector3 newPos = btVector3(x + response.x, 0.6f, z + response.z);

						newPosition = glm::vec3(x + response.x, 0.6f, z + response.z);
						


						btTransform initialTransform;
						initialTransform.setOrigin(newPos);
						m_pRigidBody->getWorldTransform().setOrigin(newPos);
						//m_pMotionState->getWorldTransform();// etOrigin(position);
						m_pGhostObject->getWorldTransform().setOrigin(newPos);

						m_pMotionState->setWorldTransform(initialTransform);




						std::cout << x + response.x << ", " << z + response.z <<"\n";

				//		std::cout << "Dist: " << pt.getDistance() << "\n";
				//		std::cout << "Lenght: " << length << "\n";
						
					//	if (entityData->name != "Floor")
						//	CheckCollisions();
						
					//	return;
						// handle collisions here
					}
				}
			}
		}

		/*
		btManifoldArray manifoldArray;
		btBroadphasePairArray& pairArray = m_pGhostObject->getOverlappingPairCache()->getOverlappingPairArray();
		int numPairs = pairArray.size();
		std::cout << " pairs: " << pairArray.size() << "\n";

		std::cout << Physics::m_rigidBodies.size() << "\n";

		for (int i = 0; i < Physics::m_rigidBodies.size(); i++)
		{
			btDrawingResult renderCallback;
			btRigidBody plyr(*m_pRigidBody);                     // reference to player
			btRigidBody body(*Physics::m_rigidBodies[i]);        // reference to object in world
			Physics::m_dynamicsWorld->contactPairTest(&plyr, &body, renderCallback);
		}
		//			
		//btGhostObject plyr(*m_pGhostObject);                 // reference to player
		*/

	/*	for (int i = 0; i < Physics::m_collisionObjects.size(); i++)
		{
			btDrawingResult renderCallback;
			btRigidBody plyr(*m_pRigidBody);                        // reference to player
			btCollisionObject body(*Physics::m_collisionObjects[i]);        // reference to object in world

			Physics::m_dynamicsWorld->contactPairTest(&plyr, &body, renderCallback);
		}*/
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