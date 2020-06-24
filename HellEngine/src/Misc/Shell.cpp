#include "hellpch.h"
#include "Shell.h"
#include "Audio/Audio.h"
#include "Helpers/AssetManager.h"
#include "Helpers/Util.h"
#include "Config.h"

namespace HellEngine
{
	std::vector<Shell> Shell::s_shells;

	Shell::Shell(Transform transform, glm::vec3 initialVelocity)
	{
		m_transform = transform;
		btVector3 collisionScale = btVector3(0.0103f * m_shellScale, 0.028f * m_shellScale, 0.0103f * m_shellScale);
		btCapsuleShape* collisionShape = new btCapsuleShape(0.5, 0.5);
	//	btCylinderShape* collisionShape = new btCylinderShape(btVector3(0.5, 0.5, 0.5));
		collisionShape->setLocalScaling(collisionScale);
		//collisionShape->setMargin(btScalar(0.04f));

		btTransform trans;
		trans.setIdentity();
		trans.setOrigin(Util::glmVec3_to_btVec3(transform.position));

		// set intial rotation based off transform param, and then rotate it 90 and 90 so it aint standing up
		glm::quat qt = glm::quat(transform.rotation);
		qt *= glm::quat(0.5, 0.5, 0.5, 0.5);
		trans.setRotation(btQuaternion(qt.x, qt.y, qt.z, qt.w));

		int group = CollisionGroups::PROJECTILES;
		int mask = CollisionGroups::HOUSE;

		float mass = 4;

		btVector3 localInertia(0, 0, 0);
		collisionShape->calculateLocalInertia(mass, localInertia);
		btDefaultMotionState* m_pMotionState = new OpenGLMotionState(trans); 


		btRigidBody::btRigidBodyConstructionInfo rbInfo(1.0f, m_pMotionState, collisionShape, localInertia);
		rbInfo.m_restitution = 1;

		//rbInfo.m_angularDamping = 0.999f;// Config::TEST_FLOAT;

		this->m_rigidBody = new btRigidBody(rbInfo);

		this->m_rigidBody->setFriction(0.7); 

	//	this->m_rigidBody->setMaxCoordinateVelocity();


		//this->m_rigidBody->setDeactivationTime(Config::TEST_FLOAT);
		//this->m_rigidBody->setSleepingThresholds(Config::TEST_FLOAT2, Config::TEST_FLOAT3);
		//this->m_rigidBody->setContactProcessingThreshold(Config::TEST_FLOAT4);

		//this->m_rigidBody->setCcdMotionThreshold(1e-6);						// This
		this->m_rigidBody->setCcdMotionThreshold(0.0103f * m_shellScale);						// This
		this->m_rigidBody->setCcdSweptSphereRadius(0.0103f * m_shellScale);	// and this, is to prevent tunneling
		this->m_rigidBody->setLinearVelocity(Util::glmVec3_to_btVec3(initialVelocity));

		EntityData* entityData = new EntityData();
		entityData->name = "SHELL";
		//entityData->vectorIndex = s_shells.size() - 1;
		m_rigidBody->setUserPointer(entityData);

		Physics::s_dynamicsWorld->addRigidBody(this->m_rigidBody, group, mask);
	}

	void Shell::Draw(Shader* shader)
	{
		static int modelID = AssetManager::GetModelIDByName("Shell");
		static int materialID = AssetManager::GetMaterialIDByName("Shell");
		AssetManager::BindMaterial(materialID);

		Transform scaleTransform;
		scaleTransform.scale = glm::vec3(m_shellScale);

		if (m_rigidBody != nullptr)
			m_modelMatrix = Physics::GetModelMatrixFromRigidBody(m_rigidBody);

		AssetManager::models[modelID].Draw(shader, m_modelMatrix * scaleTransform.to_mat4());
	}

	void Shell::Update(float deltaTime)
	{
		if (m_rigidBody == nullptr)
			return;

		// Once the shell hits the ground (UserIndex == 1), then this counter increases
		// and after a fixed time, angular velocity cuts. This stops shells rolling forever.
		if (m_rigidBody->getUserIndex() == 1)
			m_timeSinceHitTheGround += deltaTime;

		if (m_timeSinceHitTheGround > 0.15f) {
			this->m_rigidBody->setAngularVelocity(btVector3(0, 0, 0));
		}

		// Remove rigid body if at rest or fell through floor
		if ((m_rigidBody->getActivationState() != ACTIVE_TAG) || (Util::GetTranslationFromMatrix(m_modelMatrix).y < -5)) {
			Physics::DeleteRigidBody(m_rigidBody);
		}
	}
}

