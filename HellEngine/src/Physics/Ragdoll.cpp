#include "hellpch.h"
#include "Ragdoll.h"
#include "Physics.h"
#include "Helpers/Util.h"
#include <math.h>


namespace HellEngine
{
	Ragdoll::Ragdoll()
	{
	}

	Ragdoll::Ragdoll(const btVector3& positionOffset, float modelScale, glm::mat4 worldMatrix)
	{
		m_ownerWorld = Physics::s_dynamicsWorld;

		// Get world positions of model skeleton
		SkinnedModel* skinnedModel = AssetManager::skinnedModels[AssetManager::GetSkinnedModelIDByName("Zombie.fbx")];



		glm::vec3 upperarm_l = Util::GetTranslationFromMatrix(skinnedModel->m_BoneInfo[skinnedModel->m_BoneMapping["upperarm_l"]].DebugMatrix_BindPose) * modelScale;
		glm::vec3 lowerarm_l = Util::GetTranslationFromMatrix(skinnedModel->m_BoneInfo[skinnedModel->m_BoneMapping["lowerarm_l"]].DebugMatrix_BindPose) * modelScale;
		glm::vec3 hand_l = Util::GetTranslationFromMatrix(skinnedModel->m_BoneInfo[skinnedModel->m_BoneMapping["hand_l"]].DebugMatrix_BindPose) * modelScale;
	
		glm::vec3 upperarm_r = Util::GetTranslationFromMatrix(skinnedModel->m_BoneInfo[skinnedModel->m_BoneMapping["upperarm_r"]].DebugMatrix_BindPose) * modelScale;
		glm::vec3 lowerarm_r = Util::GetTranslationFromMatrix(skinnedModel->m_BoneInfo[skinnedModel->m_BoneMapping["lowerarm_r"]].DebugMatrix_BindPose) * modelScale;
		glm::vec3 hand_r = Util::GetTranslationFromMatrix(skinnedModel->m_BoneInfo[skinnedModel->m_BoneMapping["hand_r"]].DebugMatrix_BindPose) * modelScale;
		
		glm::vec3 thigh_l = Util::GetTranslationFromMatrix(skinnedModel->m_BoneInfo[skinnedModel->m_BoneMapping["thigh_l"]].DebugMatrix_BindPose) * modelScale;
		glm::vec3 calf_l = Util::GetTranslationFromMatrix(skinnedModel->m_BoneInfo[skinnedModel->m_BoneMapping["calf_l"]].DebugMatrix_BindPose) * modelScale;
		glm::vec3 foot_l = Util::GetTranslationFromMatrix(skinnedModel->m_BoneInfo[skinnedModel->m_BoneMapping["foot_l"]].DebugMatrix_BindPose) * modelScale;
	
		glm::vec3 thigh_r = Util::GetTranslationFromMatrix(skinnedModel->m_BoneInfo[skinnedModel->m_BoneMapping["thigh_r"]].DebugMatrix_BindPose) * modelScale;
		glm::vec3 calf_r = Util::GetTranslationFromMatrix(skinnedModel->m_BoneInfo[skinnedModel->m_BoneMapping["calf_r"]].DebugMatrix_BindPose) * modelScale;
		glm::vec3 foot_r = Util::GetTranslationFromMatrix(skinnedModel->m_BoneInfo[skinnedModel->m_BoneMapping["foot_r"]].DebugMatrix_BindPose) * modelScale;

		glm::vec3 pelvis = Util::GetTranslationFromMatrix(skinnedModel->m_BoneInfo[skinnedModel->m_BoneMapping["pelvis"]].DebugMatrix_BindPose) * modelScale;
		glm::vec3 spine_01 = Util::GetTranslationFromMatrix(skinnedModel->m_BoneInfo[skinnedModel->m_BoneMapping["spine_01"]].DebugMatrix_BindPose) * modelScale;
		glm::vec3 neck_01 = Util::GetTranslationFromMatrix(skinnedModel->m_BoneInfo[skinnedModel->m_BoneMapping["neck_01"]].DebugMatrix_BindPose) * modelScale;

		

		float length_upperarm = glm::length(upperarm_l - lowerarm_l);
		float length_lowerarm = glm::length(hand_l - lowerarm_l);
		float length_upperleg = glm::length(thigh_l - calf_l);
		float length_lowerleg = glm::length(calf_l - foot_l);
		float length_pelvis = glm::length(pelvis - spine_01);
		float length_spine = glm::length(spine_01 - neck_01);

		// Setup the geometry
		float scale_ragdoll = 0;
		m_shapes[BODYPART_PELVIS] = new btCapsuleShape(btScalar(1 * 0.15), btScalar(length_pelvis));
		m_shapes[BODYPART_SPINE] = new btCapsuleShape(btScalar(1 * 0.15), btScalar(length_spine));
		m_shapes[BODYPART_HEAD] = new btCapsuleShape(btScalar(scale_ragdoll * 0.10), btScalar(scale_ragdoll * 0.05));

		m_shapes[BODYPART_LEFT_UPPER_LEG] = new btCapsuleShape(btScalar(1 * 0.07), btScalar(length_upperleg));
		m_shapes[BODYPART_LEFT_LOWER_LEG] = new btCapsuleShape(btScalar(1 * 0.05), btScalar(length_lowerleg));
		m_shapes[BODYPART_RIGHT_UPPER_LEG] = new btCapsuleShape(btScalar(1 * 0.07), btScalar(length_upperleg));
		m_shapes[BODYPART_RIGHT_LOWER_LEG] = new btCapsuleShape(btScalar(1 * 0.05), btScalar(length_lowerleg));

		m_shapes[BODYPART_LEFT_UPPER_ARM] = new btCapsuleShape(btScalar(1 * 0.04), btScalar(length_upperarm));
		m_shapes[BODYPART_LEFT_LOWER_ARM] = new btCapsuleShape(btScalar(1 * 0.03), btScalar(length_lowerarm));
		m_shapes[BODYPART_RIGHT_UPPER_ARM] = new btCapsuleShape(btScalar(1 * 0.04), btScalar(length_upperarm));
		m_shapes[BODYPART_RIGHT_LOWER_ARM] = new btCapsuleShape(btScalar(1 * 0.03), btScalar(length_lowerarm));

		// Setup all the rigid bodies
		btTransform offset;
		offset.setIdentity();
		offset.setOrigin(positionOffset);

		btTransform transform;
		glm::mat4 boneMatrix;
		glm::quat rotate90 = glm::quat(0.7071, 0, 0, 0.7071);
		glm::quat q;
		btQuaternion rot;
		
		
		transform.setIdentity();
		transform.setOrigin(Util::GetRelPosBetween2Vectors(pelvis, spine_01));
		boneMatrix = skinnedModel->m_BoneInfo[skinnedModel->m_BoneMapping["pelvis"]].DebugMatrix_BindPose;
		q = glm::quat_cast(boneMatrix) * rotate90;
		rot = btQuaternion(q.x, q.y, q.z, q.w);
		transform.setRotation(rot);
		m_bodies[BODYPART_PELVIS] = localCreateRigidBody(btScalar(1.), offset * transform, m_shapes[BODYPART_PELVIS]);

		transform.setIdentity();
		transform.setOrigin(Util::GetRelPosBetween2Vectors(spine_01, neck_01));
		boneMatrix = skinnedModel->m_BoneInfo[skinnedModel->m_BoneMapping["neck_1"]].DebugMatrix_BindPose;
		boneMatrix = glm::mat4(1);
		q = glm::quat_cast(boneMatrix) * rotate90;
		rot = btQuaternion(q.x, q.y, q.z, q.w);
		transform.setRotation(rot);
		m_bodies[BODYPART_SPINE] = localCreateRigidBody(btScalar(1.), offset * transform, m_shapes[BODYPART_SPINE]);

		transform.setIdentity();
		transform.setOrigin(btVector3(btScalar(0.), btScalar(scale_ragdoll * 1.6), btScalar(0.)));
		m_bodies[BODYPART_HEAD] = localCreateRigidBody(btScalar(1.), offset * transform, m_shapes[BODYPART_HEAD]);



		transform.setIdentity();
		transform.setOrigin(Util::GetRelPosBetween2Vectors(thigh_l, calf_l));
		boneMatrix = skinnedModel->m_BoneInfo[skinnedModel->m_BoneMapping["thigh_l"]].DebugMatrix_BindPose;
		q = glm::quat_cast(boneMatrix) * rotate90;
		rot = btQuaternion(q.x, q.y, q.z, q.w);
		transform.setRotation(rot);
		m_bodies[BODYPART_LEFT_UPPER_LEG] = localCreateRigidBody(btScalar(1.), offset * transform, m_shapes[BODYPART_LEFT_UPPER_LEG]);

		transform.setIdentity();
		transform.setOrigin(Util::GetRelPosBetween2Vectors(thigh_r, calf_r));
		boneMatrix = skinnedModel->m_BoneInfo[skinnedModel->m_BoneMapping["thigh_r"]].DebugMatrix_BindPose;
		q = glm::quat_cast(boneMatrix) * rotate90;
		rot = btQuaternion(q.x, q.y, q.z, q.w);
		transform.setRotation(rot);
		m_bodies[BODYPART_RIGHT_UPPER_LEG] = localCreateRigidBody(btScalar(1.), offset * transform, m_shapes[BODYPART_RIGHT_UPPER_LEG]);



		transform.setIdentity();
		transform.setOrigin(Util::GetRelPosBetween2Vectors(foot_l, calf_l));
		boneMatrix = skinnedModel->m_BoneInfo[skinnedModel->m_BoneMapping["calf_l"]].DebugMatrix_BindPose;
		q = glm::quat_cast(boneMatrix) * rotate90;
		rot = btQuaternion(q.x, q.y, q.z, q.w);
		transform.setRotation(rot);
		m_bodies[BODYPART_LEFT_LOWER_LEG] = localCreateRigidBody(btScalar(1.), offset * transform, m_shapes[BODYPART_LEFT_LOWER_LEG]);

		transform.setIdentity();
		transform.setOrigin(Util::GetRelPosBetween2Vectors(foot_r, calf_r));
		boneMatrix = skinnedModel->m_BoneInfo[skinnedModel->m_BoneMapping["calf_r"]].DebugMatrix_BindPose;
		q = glm::quat_cast(boneMatrix) * rotate90;
		rot = btQuaternion(q.x, q.y, q.z, q.w);
		transform.setRotation(rot);
		m_bodies[BODYPART_RIGHT_LOWER_LEG] = localCreateRigidBody(btScalar(1.), offset * transform, m_shapes[BODYPART_RIGHT_LOWER_LEG]);




		transform.setIdentity();
		transform.setOrigin(Util::GetRelPosBetween2Vectors(lowerarm_l, upperarm_l));
		boneMatrix = skinnedModel->m_BoneInfo[skinnedModel->m_BoneMapping["upperarm_l"]].DebugMatrix_BindPose;
		q = glm::quat_cast(boneMatrix) * rotate90;
		rot = btQuaternion(q.x, q.y, q.z, q.w);
		transform.setRotation(rot);
		m_bodies[BODYPART_LEFT_UPPER_ARM] = localCreateRigidBody(btScalar(1.), offset * transform, m_shapes[BODYPART_LEFT_UPPER_ARM]);

		transform.setIdentity();
		transform.setOrigin(Util::GetRelPosBetween2Vectors(hand_l, lowerarm_l));
		boneMatrix = skinnedModel->m_BoneInfo[skinnedModel->m_BoneMapping["lowerarm_l"]].DebugMatrix_BindPose;
		q = glm::quat_cast(boneMatrix) * rotate90;
		rot = btQuaternion(q.x, q.y, q.z, q.w);
		transform.setRotation(rot);
		m_bodies[BODYPART_LEFT_LOWER_ARM] = localCreateRigidBody(btScalar(1.), offset * transform, m_shapes[BODYPART_LEFT_LOWER_ARM]);


		transform.setIdentity();
		transform.setOrigin(Util::GetRelPosBetween2Vectors(lowerarm_r, upperarm_r));
		boneMatrix = skinnedModel->m_BoneInfo[skinnedModel->m_BoneMapping["upperarm_r"]].DebugMatrix_BindPose;
		q = glm::quat_cast(boneMatrix) * rotate90;
		rot = btQuaternion(q.x, q.y, q.z, q.w);
		transform.setRotation(rot);
		m_bodies[BODYPART_RIGHT_UPPER_ARM] = localCreateRigidBody(btScalar(1.), offset * transform, m_shapes[BODYPART_RIGHT_UPPER_ARM]);

		transform.setIdentity();
		transform.setOrigin(Util::GetRelPosBetween2Vectors(hand_r, lowerarm_r));
		boneMatrix = skinnedModel->m_BoneInfo[skinnedModel->m_BoneMapping["lowerarm_r"]].DebugMatrix_BindPose;
		q = glm::quat_cast(boneMatrix) * rotate90;
		rot = btQuaternion(q.x, q.y, q.z, q.w);
		transform.setRotation(rot);
		m_bodies[BODYPART_RIGHT_LOWER_ARM] = localCreateRigidBody(btScalar(1.), offset * transform, m_shapes[BODYPART_RIGHT_LOWER_ARM]);


		// Setup some damping on the m_bodies
		for (int i = 0; i < BODYPART_COUNT; ++i)
		{
			m_bodies[i]->setDamping(0.05f, 0.85f);
			m_bodies[i]->setDeactivationTime(0.8f);
			m_bodies[i]->setSleepingThresholds(1.6f, 2.5f);
			m_bodies[i]->setActivationState(0);//
		}

		scale_ragdoll = 1;

		///////////////////////////// SETTING THE CONSTRAINTS /////////////////////////////////////////////7777
			// Now setup the constraints
		btGeneric6DofConstraint* joint6DOF;
		btTransform localA, localB;
		bool useLinearReferenceFrameA = true;
		/// ******* SPINE HEAD ******** ///
		{
			localA.setIdentity(); localB.setIdentity();

			localA.setOrigin(btVector3(btScalar(0.), btScalar(0.30 * scale_ragdoll), btScalar(0.)));

			localB.setOrigin(btVector3(btScalar(0.), btScalar(-0.14 * scale_ragdoll), btScalar(0.)));

			joint6DOF = new btGeneric6DofConstraint(*m_bodies[BODYPART_SPINE], *m_bodies[BODYPART_HEAD], localA, localB, useLinearReferenceFrameA);

#ifdef RIGID
			joint6DOF->setAngularLowerLimit(btVector3(-SIMD_EPSILON, -SIMD_EPSILON, -SIMD_EPSILON));
			joint6DOF->setAngularUpperLimit(btVector3(SIMD_EPSILON, SIMD_EPSILON, SIMD_EPSILON));
#else
			joint6DOF->setAngularLowerLimit(btVector3(-SIMD_PI * 0.3f, -SIMD_EPSILON, -SIMD_PI * 0.3f));
			joint6DOF->setAngularUpperLimit(btVector3(SIMD_PI * 0.5f, SIMD_EPSILON, SIMD_PI * 0.3f));
#endif
			m_joints[JOINT_SPINE_HEAD] = joint6DOF;
			m_ownerWorld->addConstraint(m_joints[JOINT_SPINE_HEAD], true);

		}
		/// *************************** ///




		/// ******* LEFT SHOULDER ******** ///
		{
			localA.setIdentity(); localB.setIdentity();

			localA.setOrigin(btVector3(btScalar(-0.2 * scale_ragdoll), btScalar(0.15 * scale_ragdoll), btScalar(0.)));

			localB.getBasis().setEulerZYX(SIMD_HALF_PI, 0, -SIMD_HALF_PI);
			localB.setOrigin(btVector3(btScalar(0.), btScalar(-0.18 * scale_ragdoll), btScalar(0.)));

			joint6DOF = new btGeneric6DofConstraint(*m_bodies[BODYPART_SPINE], *m_bodies[BODYPART_LEFT_UPPER_ARM], localA, localB, useLinearReferenceFrameA);

#ifdef RIGID
			joint6DOF->setAngularLowerLimit(btVector3(-SIMD_EPSILON, -SIMD_EPSILON, -SIMD_EPSILON));
			joint6DOF->setAngularUpperLimit(btVector3(SIMD_EPSILON, SIMD_EPSILON, SIMD_EPSILON));
#else
			joint6DOF->setAngularLowerLimit(btVector3(-SIMD_PI * 0.8f, -SIMD_EPSILON, -SIMD_PI * 0.5f));
			joint6DOF->setAngularUpperLimit(btVector3(SIMD_PI * 0.8f, SIMD_EPSILON, SIMD_PI * 0.5f));
#endif
			m_joints[JOINT_LEFT_SHOULDER] = joint6DOF;
			m_ownerWorld->addConstraint(m_joints[JOINT_LEFT_SHOULDER], true);
		}
		/// *************************** ///


		/// ******* RIGHT SHOULDER ******** ///
		{
			localA.setIdentity(); localB.setIdentity();

			localA.setOrigin(btVector3(btScalar(0.2 * scale_ragdoll), btScalar(0.15 * scale_ragdoll), btScalar(0.)));
			localB.getBasis().setEulerZYX(0, 0, SIMD_HALF_PI);
			localB.setOrigin(btVector3(btScalar(0.), btScalar(-0.18 * scale_ragdoll), btScalar(0.)));
			joint6DOF = new btGeneric6DofConstraint(*m_bodies[BODYPART_SPINE], *m_bodies[BODYPART_RIGHT_UPPER_ARM], localA, localB, useLinearReferenceFrameA);

#ifdef RIGID
			joint6DOF->setAngularLowerLimit(btVector3(-SIMD_EPSILON, -SIMD_EPSILON, -SIMD_EPSILON));
			joint6DOF->setAngularUpperLimit(btVector3(SIMD_EPSILON, SIMD_EPSILON, SIMD_EPSILON));
#else

			joint6DOF->setAngularLowerLimit(btVector3(-SIMD_PI * 0.8f, -SIMD_EPSILON, -SIMD_PI * 0.5f));
			joint6DOF->setAngularUpperLimit(btVector3(SIMD_PI * 0.8f, SIMD_EPSILON, SIMD_PI * 0.5f));
#endif
			m_joints[JOINT_RIGHT_SHOULDER] = joint6DOF;
			m_ownerWorld->addConstraint(m_joints[JOINT_RIGHT_SHOULDER], true);
		}
		/// *************************** ///

		/// ******* LEFT ELBOW ******** ///
		{
			localA.setIdentity(); localB.setIdentity();

			localA.setOrigin(btVector3(btScalar(0.), btScalar(0.18 * scale_ragdoll), btScalar(0.)));
			localB.setOrigin(btVector3(btScalar(0.), btScalar(-0.14 * scale_ragdoll), btScalar(0.)));
			joint6DOF = new btGeneric6DofConstraint(*m_bodies[BODYPART_LEFT_UPPER_ARM], *m_bodies[BODYPART_LEFT_LOWER_ARM], localA, localB, useLinearReferenceFrameA);

#ifdef RIGID
			joint6DOF->setAngularLowerLimit(btVector3(-SIMD_EPSILON, -SIMD_EPSILON, -SIMD_EPSILON));
			joint6DOF->setAngularUpperLimit(btVector3(SIMD_EPSILON, SIMD_EPSILON, SIMD_EPSILON));
#else
			joint6DOF->setAngularLowerLimit(btVector3(-SIMD_EPSILON, -SIMD_EPSILON, -SIMD_EPSILON));
			joint6DOF->setAngularUpperLimit(btVector3(SIMD_PI * 0.7f, SIMD_EPSILON, SIMD_EPSILON));
#endif
			m_joints[JOINT_LEFT_ELBOW] = joint6DOF;
			m_ownerWorld->addConstraint(m_joints[JOINT_LEFT_ELBOW], true);
		}
		/// *************************** ///

		/// ******* RIGHT ELBOW ******** ///
		{
			localA.setIdentity(); localB.setIdentity();

			localA.setOrigin(btVector3(btScalar(0.), btScalar(0.18 * scale_ragdoll), btScalar(0.)));
			localB.setOrigin(btVector3(btScalar(0.), btScalar(-0.14 * scale_ragdoll), btScalar(0.)));
			joint6DOF = new btGeneric6DofConstraint(*m_bodies[BODYPART_RIGHT_UPPER_ARM], *m_bodies[BODYPART_RIGHT_LOWER_ARM], localA, localB, useLinearReferenceFrameA);

#ifdef RIGID
			joint6DOF->setAngularLowerLimit(btVector3(-SIMD_EPSILON, -SIMD_EPSILON, -SIMD_EPSILON));
			joint6DOF->setAngularUpperLimit(btVector3(SIMD_EPSILON, SIMD_EPSILON, SIMD_EPSILON));
#else
			joint6DOF->setAngularLowerLimit(btVector3(-SIMD_EPSILON, -SIMD_EPSILON, -SIMD_EPSILON));
			joint6DOF->setAngularUpperLimit(btVector3(SIMD_PI * 0.7, SIMD_EPSILON, SIMD_EPSILON));
#endif

			m_joints[JOINT_RIGHT_ELBOW] = joint6DOF;
			m_ownerWorld->addConstraint(m_joints[JOINT_RIGHT_ELBOW], true);
		}
		/// *************************** ///


		/// ******* PELVIS ******** ///
		{
			localA.setIdentity(); localB.setIdentity();

			localA.getBasis().setEulerZYX(0, SIMD_HALF_PI, 0);
			localA.setOrigin(btVector3(btScalar(0.), btScalar(0.15 * scale_ragdoll), btScalar(0.)));
			localB.getBasis().setEulerZYX(0, SIMD_HALF_PI, 0);
			localB.setOrigin(btVector3(btScalar(0.), btScalar(-0.15 * scale_ragdoll), btScalar(0.)));
			joint6DOF = new btGeneric6DofConstraint(*m_bodies[BODYPART_PELVIS], *m_bodies[BODYPART_SPINE], localA, localB, useLinearReferenceFrameA);

#ifdef RIGID
			joint6DOF->setAngularLowerLimit(btVector3(-SIMD_EPSILON, -SIMD_EPSILON, -SIMD_EPSILON));
			joint6DOF->setAngularUpperLimit(btVector3(SIMD_EPSILON, SIMD_EPSILON, SIMD_EPSILON));
#else
			joint6DOF->setAngularLowerLimit(btVector3(-SIMD_PI * 0.2, -SIMD_EPSILON, -SIMD_PI * 0.3));
			joint6DOF->setAngularUpperLimit(btVector3(SIMD_PI * 0.2, SIMD_EPSILON, SIMD_PI * 0.6));
#endif
			m_joints[JOINT_PELVIS_SPINE] = joint6DOF;
			m_ownerWorld->addConstraint(m_joints[JOINT_PELVIS_SPINE], true);
		}
		/// *************************** ///

		/// ******* LEFT HIP ******** ///
		{
			localA.setIdentity(); localB.setIdentity();

			localA.setOrigin(btVector3(btScalar(-0.18 * scale_ragdoll), btScalar(-0.10 * scale_ragdoll), btScalar(0.)));

			localB.setOrigin(btVector3(btScalar(0.), btScalar(0.225 * scale_ragdoll), btScalar(0.)));

			joint6DOF = new btGeneric6DofConstraint(*m_bodies[BODYPART_PELVIS], *m_bodies[BODYPART_LEFT_UPPER_LEG], localA, localB, useLinearReferenceFrameA);

#ifdef RIGID
			joint6DOF->setAngularLowerLimit(btVector3(-SIMD_EPSILON, -SIMD_EPSILON, -SIMD_EPSILON));
			joint6DOF->setAngularUpperLimit(btVector3(SIMD_EPSILON, SIMD_EPSILON, SIMD_EPSILON));
#else
			joint6DOF->setAngularLowerLimit(btVector3(-SIMD_HALF_PI * 0.5, -SIMD_EPSILON, -SIMD_EPSILON));
			joint6DOF->setAngularUpperLimit(btVector3(SIMD_HALF_PI * 0.8, SIMD_EPSILON, SIMD_HALF_PI * 0.6f));
#endif
			m_joints[JOINT_LEFT_HIP] = joint6DOF;
			m_ownerWorld->addConstraint(m_joints[JOINT_LEFT_HIP], true);
		}
		/// *************************** ///


		/// ******* RIGHT HIP ******** ///
		{
			localA.setIdentity(); localB.setIdentity();

			localA.setOrigin(btVector3(btScalar(0.18 * scale_ragdoll), btScalar(-0.10 * scale_ragdoll), btScalar(0.)));
			localB.setOrigin(btVector3(btScalar(0.), btScalar(0.225 * scale_ragdoll), btScalar(0.)));

			joint6DOF = new btGeneric6DofConstraint(*m_bodies[BODYPART_PELVIS], *m_bodies[BODYPART_RIGHT_UPPER_LEG], localA, localB, useLinearReferenceFrameA);

#ifdef RIGID
			joint6DOF->setAngularLowerLimit(btVector3(-SIMD_EPSILON, -SIMD_EPSILON, -SIMD_EPSILON));
			joint6DOF->setAngularUpperLimit(btVector3(SIMD_EPSILON, SIMD_EPSILON, SIMD_EPSILON));
#else
			joint6DOF->setAngularLowerLimit(btVector3(-SIMD_HALF_PI * 0.5, -SIMD_EPSILON, -SIMD_HALF_PI * 0.6f));
			joint6DOF->setAngularUpperLimit(btVector3(SIMD_HALF_PI * 0.8, SIMD_EPSILON, SIMD_EPSILON));
#endif
			m_joints[JOINT_RIGHT_HIP] = joint6DOF;
			m_ownerWorld->addConstraint(m_joints[JOINT_RIGHT_HIP], true);
		}
		/// *************************** ///


		/// ******* LEFT KNEE ******** ///
		{
			localA.setIdentity(); localB.setIdentity();

			localA.setOrigin(btVector3(btScalar(0.), btScalar(-0.225 * scale_ragdoll), btScalar(0.)));
			localB.setOrigin(btVector3(btScalar(0.), btScalar(0.185 * scale_ragdoll), btScalar(0.)));
			joint6DOF = new btGeneric6DofConstraint(*m_bodies[BODYPART_LEFT_UPPER_LEG], *m_bodies[BODYPART_LEFT_LOWER_LEG], localA, localB, useLinearReferenceFrameA);
			//
#ifdef RIGID
			joint6DOF->setAngularLowerLimit(btVector3(-SIMD_EPSILON, -SIMD_EPSILON, -SIMD_EPSILON));
			joint6DOF->setAngularUpperLimit(btVector3(SIMD_EPSILON, SIMD_EPSILON, SIMD_EPSILON));
#else
			joint6DOF->setAngularLowerLimit(btVector3(-SIMD_EPSILON, -SIMD_EPSILON, -SIMD_EPSILON));
			joint6DOF->setAngularUpperLimit(btVector3(SIMD_PI * 0.7f, SIMD_EPSILON, SIMD_EPSILON));
#endif
			m_joints[JOINT_LEFT_KNEE] = joint6DOF;
			m_ownerWorld->addConstraint(m_joints[JOINT_LEFT_KNEE], true);
		}
		/// *************************** ///

		/// ******* RIGHT KNEE ******** ///
		{
			localA.setIdentity(); localB.setIdentity();

			localA.setOrigin(btVector3(btScalar(0.), btScalar(-0.225 * scale_ragdoll), btScalar(0.)));
			localB.setOrigin(btVector3(btScalar(0.), btScalar(0.185 * scale_ragdoll), btScalar(0.)));
			joint6DOF = new btGeneric6DofConstraint(*m_bodies[BODYPART_RIGHT_UPPER_LEG], *m_bodies[BODYPART_RIGHT_LOWER_LEG], localA, localB, useLinearReferenceFrameA);

#ifdef RIGID
			joint6DOF->setAngularLowerLimit(btVector3(-SIMD_EPSILON, -SIMD_EPSILON, -SIMD_EPSILON));
			joint6DOF->setAngularUpperLimit(btVector3(SIMD_EPSILON, SIMD_EPSILON, SIMD_EPSILON));
#else
			joint6DOF->setAngularLowerLimit(btVector3(-SIMD_EPSILON, -SIMD_EPSILON, -SIMD_EPSILON));
			joint6DOF->setAngularUpperLimit(btVector3(SIMD_PI * 0.7f, SIMD_EPSILON, SIMD_EPSILON));
#endif
			m_joints[JOINT_RIGHT_KNEE] = joint6DOF;
			m_ownerWorld->addConstraint(m_joints[JOINT_RIGHT_KNEE], true);
		}
		/// *************************** ///

	}


	Ragdoll::~Ragdoll()
	{
		int i;

		// Remove all constraints
		for (i = 0; i < JOINT_COUNT; ++i)
		{
			m_ownerWorld->removeConstraint(m_joints[i]);
			delete m_joints[i]; m_joints[i] = 0;
		}

		// Remove all bodies and shapes
		for (i = 0; i < BODYPART_COUNT; ++i)
		{
			m_ownerWorld->removeRigidBody(m_bodies[i]);

			delete m_bodies[i]->getMotionState();

			delete m_bodies[i]; m_bodies[i] = 0;
			delete m_shapes[i]; m_shapes[i] = 0;
		}
	}


	btRigidBody* Ragdoll::localCreateRigidBody(btScalar mass, const btTransform& startTransform, btCollisionShape* shape)
	{
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			shape->calculateLocalInertia(mass, localInertia);

		btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, shape, localInertia);
		rbInfo.m_additionalDamping = true;
		btRigidBody* body = new btRigidBody(rbInfo);


		int group = CollisionGroups::ENEMY;
		int mask = CollisionGroups::HOUSE | CollisionGroups::PLAYER;


		m_ownerWorld->addRigidBody(body, group, mask);

		return body;
	}
}