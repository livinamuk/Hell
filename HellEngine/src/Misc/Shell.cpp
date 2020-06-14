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
		btVector3 collisionScale = btVector3(0.0103f * shellScale, 0.028f * shellScale, 0.0103f * shellScale);
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

		//float mass = Config::TEST_FLOAT3;
		float mass = 4;//

		btVector3 localInertia(0, 0, 0);
		collisionShape->calculateLocalInertia(mass, localInertia);
		btDefaultMotionState* m_pMotionState = new OpenGLMotionState(trans); 


		btRigidBody::btRigidBodyConstructionInfo rbInfo(1.0f, m_pMotionState, collisionShape, localInertia);
		rbInfo.m_restitution = 1;

		//rbInfo.m_angularDamping = Config::TEST_FLOAT;
		rbInfo.m_angularDamping = 0.999f;// Config::TEST_FLOAT;

		this->m_rigidBody = new btRigidBody(rbInfo);

		this->m_rigidBody->setFriction(0.7); 
		//this->m_rigidBody->setFriction(Config::TEST_FLOAT2);

		//this->m_rigidBody->setCcdMotionThreshold(1e-6);						// This
		this->m_rigidBody->setCcdMotionThreshold(0.0103f * shellScale);						// This
		this->m_rigidBody->setCcdSweptSphereRadius(0.0103f * shellScale);	// and this, is to prevent tunneling
		this->m_rigidBody->setLinearVelocity(Util::glmVec3_to_btVec3(initialVelocity));

		EntityData* entityData = new EntityData();
		entityData->name = "SHELL";
		m_rigidBody->setUserPointer(entityData);

		Physics::s_dynamicsWorld->addRigidBody(this->m_rigidBody, group, mask);
	}

	void Shell::Draw(Shader* shader)
	{
		static int modelID = AssetManager::GetModelIDByName("Shell");
		static int materialID = AssetManager::GetMaterialIDByName("Shell");
		AssetManager::BindMaterial(materialID);

		Transform trans;
		trans.scale = glm::vec3(shellScale);

		glm::mat4 modelMatrix = Physics::GetModelMatrixFromRigidBody(m_rigidBody);
		AssetManager::models[modelID].Draw(shader, modelMatrix * trans.to_mat4());
	}

	void Shell::Update(float deltaTime)
	{
		/*if (activePhysics)
		{
			shellVelocity.y -= shellGravity * deltaTime;
			transform.position += shellVelocity;
		}

		if (transform.position.y < 0)
		{
			Audio::PlayAudio("ShellBounce.wav");
			activePhysics = false;
			transform.position.y = 0.01f;
			shellVelocity = glm::vec3(0);
			transform.rotation.x = 0;
		}
		//	std::max(transform.position.y, 0.0f);*/
	}
	/*
	void Shell::Init()
	{
		float shellForwardFactor = 0.25f;
		float shellUpFactor = 0.1;
		float shellRightFactor = 0.1f;
		glm::vec3 shellVelocity = glm::vec3(0);
		float shellSpeedRight = 1; // * 0.17
		float shellSpeedUp = 1;
		float shellGravity = 0.1f;
	}*/
}

