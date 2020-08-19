#include "hellpch.h"
#include "Shell.h"
#include "Audio/Audio.h"
#include "Helpers/AssetManager.h"
#include "Helpers/Util.h"
#include "Config.h"
#include "GL/GpuProfiling.h"

namespace HellEngine
{
	std::vector<Shell> Shell::s_shotgunShells;
	std::vector<Shell> Shell::s_bulletCasings;
	unsigned int Shell::buffer;

	Shell::Shell(Transform transform, glm::vec3 initialVelocity, CasingType casingType)
	{
		m_casingType = casingType;
		m_transform = transform;
		glGenBuffers(1, &buffer);

		if (m_casingType == CasingType::BULLET_CASING)
			m_shellScale = 1;
		if (m_casingType == CasingType::SHOTGUN_SHELL)
			m_shellScale = 1.6f;

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
		int mask = CollisionGroups::HOUSE | CollisionGroups::TERRAIN;

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
		entityData->type = PhysicsObjectType::SHELL_PROJECTILE;
		entityData->enumValue = m_casingType;
		//entityData->vectorIndex = s_shells.size() - 1;
		m_rigidBody->setUserPointer(entityData);

		Physics::s_dynamicsWorld->addRigidBody(this->m_rigidBody, group, mask);
	}

	void Shell::Draw(Shader* shader)
	{
		GpuProfiler g("Shell");
		static int Shell_ModelID = AssetManager::GetModelIDByName("Shell");
		static int Shell_MaterialID = AssetManager::GetMaterialIDByName("Shell");
		static int BulletCasing_ModelID = AssetManager::GetModelIDByName("BulletCasing");
		static int BulletCasing_MaterialID = AssetManager::GetMaterialIDByName("BulletCasing");

		Transform scaleTransform;
		scaleTransform.scale = glm::vec3(m_shellScale);


		// if its a bullet casing, make it bigger. they look too big coming out the gun but too small on the ground...
		if (m_timeSinceHitTheGround > 0 && m_casingType == CasingType::BULLET_CASING)
			scaleTransform.scale = glm::vec3(1.5f);

		if (m_rigidBody != nullptr)
			m_modelMatrix = Physics::GetModelMatrixFromRigidBody(m_rigidBody);

		if (m_casingType == CasingType::SHOTGUN_SHELL) {
			AssetManager::BindMaterial_0(Shell_MaterialID);
			AssetManager::models[Shell_ModelID].Draw(shader, m_modelMatrix * scaleTransform.to_mat4());
		}
		else if (m_casingType == CasingType::BULLET_CASING) {
			AssetManager::BindMaterial_0(BulletCasing_MaterialID);
			AssetManager::models[BulletCasing_ModelID].Draw(shader, m_modelMatrix * scaleTransform.to_mat4());
		}
	}

	void Shell::DrawInstanced(Shader* shader, std::vector<Shell>& shells)
	{
		// No shells? Then get outta here.
		if (!shells.size())
			return;

		GpuProfiler g("Shell");
		static int Shell_ModelID = AssetManager::GetModelIDByName("Shell");
		static int Shell_MaterialID = AssetManager::GetMaterialIDByName("Shell");
		static int BulletCasing_ModelID = AssetManager::GetModelIDByName("BulletCasing");
		static int BulletCasing_MaterialID = AssetManager::GetMaterialIDByName("BulletCasing");

		std::vector<glm::mat4> modelMatrixVector;

		for (Shell& shell : shells)
		{
			// If the projectile is still being simulated by the physics world then retrieve it's updated model matrix.
			if (shell.m_rigidBody != nullptr)
				shell.m_modelMatrix = Physics::GetModelMatrixFromRigidBody(shell.m_rigidBody);

			glm::mat4 scaleMatrix = glm::scale(glm::mat4(1), glm::vec3(shell.m_shellScale));
			modelMatrixVector.push_back(shell.m_modelMatrix * scaleMatrix);
		}

		// Check the projectile type.
		int modelID, materialID;
		if (shells[0].m_casingType == CasingType::BULLET_CASING) {
			modelID = BulletCasing_ModelID;
			materialID = BulletCasing_MaterialID;
		}
		if (shells[0].m_casingType == CasingType::SHOTGUN_SHELL) {
			modelID = Shell_ModelID;
			materialID = Shell_MaterialID;
		}
		AssetManager::BindMaterial_0(materialID);

		//setup vertex buffers and vertex array for model matrices
		// vertex buffer object -- should this be created every time?! very stupid question i think .. it keeps slowing the fps down!
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, shells.size() * sizeof(glm::mat4), &modelMatrixVector[0], GL_STATIC_DRAW);

		unsigned int VAO = AssetManager::models[modelID].m_meshes[0]->VAO;
		glBindVertexArray(VAO);
		// vertex attributes
		std::size_t vec4Size = sizeof(glm::vec4);
		glEnableVertexAttribArray(8);
		glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
		glEnableVertexAttribArray(9);
		glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
		glEnableVertexAttribArray(10);
		glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
		glEnableVertexAttribArray(11);
		glVertexAttribPointer(11, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));
		glVertexAttribDivisor(8, 1);
		glVertexAttribDivisor(9, 1);
		glVertexAttribDivisor(10, 1);
		glVertexAttribDivisor(11, 1);

		//draw
		glUniform1i(glGetUniformLocation(shader->ID, "instanced"), 1);
		glBindVertexArray(AssetManager::models[modelID].m_meshes[0]->VAO);
		glDrawElementsInstanced(GL_TRIANGLES, AssetManager::models[modelID].m_meshes[0]->indices.size(), GL_UNSIGNED_INT, 0, shells.size());
		glUniform1i(glGetUniformLocation(shader->ID, "instanced"), 0);
	}


	void Shell::Update(float deltaTime)
	{
		// If it's been removed from the phyics world then don't fuck with it.
		if (m_rigidBody == nullptr)
			return;

		// Remove rigid body after 2 seconds - should be enough time to have hit the floor
		m_lifeTime += deltaTime;
		if (m_lifeTime > 2) {
			Physics::DeleteRigidBody(m_rigidBody);
			return;
		}
		// Once the shell hits the ground (UserIndex == 1), then this counter increases
		// and after a fixed time, angular velocity cuts. This stops shells rolling forever.
		if (m_rigidBody->getUserIndex() == 1) {
			m_timeSinceHitTheGround += deltaTime;
		}
		if (m_timeSinceHitTheGround > 0.15f) {
			this->m_rigidBody->setAngularVelocity(btVector3(0, 0, 0));
		}

		// If it's a bullet casing from the glock, make it bigger when it hits the ground. 
		// They looked too big coming out the gun so you made them smaller remember, 
		// But now they are too small on the ground, so fix that here:
		if (m_timeSinceHitTheGround > 0 && m_casingType == CasingType::BULLET_CASING)
			m_shellScale = 1.6f;

		// Remove rigid body if at rest or fell through floor
		if ((m_rigidBody->getActivationState() != ACTIVE_TAG) || (Util::GetTranslationFromMatrix(m_modelMatrix).y < -5)) {
			Physics::DeleteRigidBody(m_rigidBody);
		}
	}
}

