#include "hellpch.h"
#include "Shell.h"
#include "Audio/Audio.h"
#include "Helpers/AssetManager.h"
#include "Helpers/Util.h"

namespace HellEngine
{
	Shell::Shell(Transform transform)
	{
		m_transform = transform;
		;// m_transform.scale = glm::vec3(0.1f);

		btVector3 collisionScale = btVector3(0.0103f, 0.028f, 0.0103f);

	//	m_transform.scale *= 0.5f;
	//	collisionScale *= 0.5f;
		
		btCylinderShape* collisionShape = new btCylinderShape(btVector3(0.5, 0.5, 0.5));
		collisionShape->setLocalScaling(collisionScale);

		btTransform trans;
		trans.setIdentity();
		trans.setOrigin(Util::glmVec3_to_btVec3(transform.position));

		int group = CollisionGroups::PROJECTILES;
		int mask = CollisionGroups::HOUSE;


		float mass = 0.4f;
		float friction = 0.5f;		
		bool isDynamic = (mass != 0.f); //rigidbody is dynamic if and only if mass is non zero, otherwise static

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			collisionShape->calculateLocalInertia(mass, localInertia);

		btDefaultMotionState* m_pMotionState = new OpenGLMotionState(trans);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(1.0f, m_pMotionState, collisionShape);
		this->m_rigidBody = new btRigidBody(rbInfo);
		Physics::s_dynamicsWorld->addRigidBody(this->m_rigidBody, group, mask);
	}

	void Shell::Draw(Shader* shader)
	{
		static int modelID = AssetManager::GetModelIDByName("Shell");
		static int materialID = AssetManager::GetMaterialIDByName("Shell");
		AssetManager::BindMaterial(materialID);

		glm::mat4 modelMatrix = Physics::GetModelMatrixFromRigidBody(m_rigidBody);
		AssetManager::models[modelID].Draw(shader, modelMatrix);
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

