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

	glm::quat fromaxisangle(float angle, glm::vec3 axis)
	{
		float half_sin = sin(0.5f * angle);
		float half_cos = cos(0.5f * angle);
		return glm::quat(half_cos,
			half_sin * axis.x,
			half_sin * axis.y,
			half_sin * axis.z);
	}

	Ragdoll::Ragdoll(const btVector3& positionOffset, float modelScale, glm::mat4 worldMatrix)
	{
		m_ownerWorld = Physics::s_dynamicsWorld;



		// Get world positions of model skeleton
		SkinnedModel* skinnedModel = AssetManager::skinnedModels[AssetManager::GetSkinnedModelIDByName("Zombie.fbx")];
		std::vector<SkinnedModel::BoneInfo> boneInfo = skinnedModel->m_BoneInfo;

		worldMatrix = glm::mat4(1);
		modelScale = 1;

		glm::vec3 upperarm_l = Util::GetTranslationFromMatrix(worldMatrix * boneInfo[skinnedModel->m_BoneMapping["upperarm_l"]].ModelSpace_AnimatedTransform) * modelScale;
		glm::vec3 lowerarm_l = Util::GetTranslationFromMatrix(worldMatrix * boneInfo[skinnedModel->m_BoneMapping["lowerarm_l"]].ModelSpace_AnimatedTransform) * modelScale;
		glm::vec3 hand_l = Util::GetTranslationFromMatrix(worldMatrix* boneInfo[skinnedModel->m_BoneMapping["hand_l"]].ModelSpace_AnimatedTransform) * modelScale;
	
		glm::vec3 upperarm_r = Util::GetTranslationFromMatrix(worldMatrix * boneInfo[skinnedModel->m_BoneMapping["upperarm_r"]].ModelSpace_AnimatedTransform) * modelScale;
		glm::vec3 lowerarm_r = Util::GetTranslationFromMatrix(worldMatrix * boneInfo[skinnedModel->m_BoneMapping["lowerarm_r"]].ModelSpace_AnimatedTransform) * modelScale;
		glm::vec3 hand_r = Util::GetTranslationFromMatrix(worldMatrix * boneInfo[skinnedModel->m_BoneMapping["hand_r"]].ModelSpace_AnimatedTransform) * modelScale;
		
		glm::vec3 thigh_l = Util::GetTranslationFromMatrix(worldMatrix * boneInfo[skinnedModel->m_BoneMapping["thigh_l"]].ModelSpace_AnimatedTransform) * modelScale;
		glm::vec3 calf_l = Util::GetTranslationFromMatrix(worldMatrix * boneInfo[skinnedModel->m_BoneMapping["calf_l"]].ModelSpace_AnimatedTransform) * modelScale;
		glm::vec3 foot_l = Util::GetTranslationFromMatrix(worldMatrix * boneInfo[skinnedModel->m_BoneMapping["foot_l"]].ModelSpace_AnimatedTransform) * modelScale;
	
		glm::vec3 thigh_r = Util::GetTranslationFromMatrix(worldMatrix * boneInfo[skinnedModel->m_BoneMapping["thigh_r"]].ModelSpace_AnimatedTransform) * modelScale;
		glm::vec3 calf_r = Util::GetTranslationFromMatrix(worldMatrix * boneInfo[skinnedModel->m_BoneMapping["calf_r"]].ModelSpace_AnimatedTransform) * modelScale;
		glm::vec3 foot_r = Util::GetTranslationFromMatrix(worldMatrix * boneInfo[skinnedModel->m_BoneMapping["foot_r"]].ModelSpace_AnimatedTransform) * modelScale;

		glm::vec3 pelvis = Util::GetTranslationFromMatrix(worldMatrix * boneInfo[skinnedModel->m_BoneMapping["pelvis"]].ModelSpace_AnimatedTransform) * modelScale;
		glm::vec3 spine_01 = Util::GetTranslationFromMatrix(worldMatrix * boneInfo[skinnedModel->m_BoneMapping["spine_01"]].ModelSpace_AnimatedTransform) * modelScale;
		glm::vec3 spine_02 = Util::GetTranslationFromMatrix(worldMatrix * boneInfo[skinnedModel->m_BoneMapping["spine_02"]].ModelSpace_AnimatedTransform) * modelScale;
		glm::vec3 spine_03 = Util::GetTranslationFromMatrix(worldMatrix * boneInfo[skinnedModel->m_BoneMapping["spine_03"]].ModelSpace_AnimatedTransform) * modelScale;

		glm::vec3 neck_01 = Util::GetTranslationFromMatrix(worldMatrix * boneInfo[skinnedModel->m_BoneMapping["neck_01"]].ModelSpace_AnimatedTransform) * modelScale;
		
		glm::vec3 Eye_l = Util::GetTranslationFromMatrix(worldMatrix * boneInfo[skinnedModel->m_BoneMapping["Eye_l"]].ModelSpace_AnimatedTransform) * modelScale;
		glm::vec3 Eye_r = Util::GetTranslationFromMatrix(worldMatrix * boneInfo[skinnedModel->m_BoneMapping["Eye_r"]].ModelSpace_AnimatedTransform) * modelScale;
		glm::vec3 head = Util::GetTranslationFromMatrix(worldMatrix * boneInfo[skinnedModel->m_BoneMapping["head"]].ModelSpace_AnimatedTransform) * modelScale;

		
		m_capsuleLengths[BODYPART_PELVIS] = glm::length(pelvis - spine_01);

		m_capsuleLengths[BODYPART_SPINE_01] = glm::length(spine_01 - spine_02);
		m_capsuleLengths[BODYPART_SPINE_02] = glm::length(spine_02 - spine_03);
		m_capsuleLengths[BODYPART_SPINE_03] = glm::length(spine_03 - neck_01);

		m_capsuleLengths[BODYPART_NECK] = glm::length(head - neck_01);
		m_capsuleLengths[BODYPART_HEAD] = 0.01f;

		m_capsuleLengths[BODYPART_LEFT_UPPER_LEG] = glm::length(thigh_l - calf_l);
		m_capsuleLengths[BODYPART_LEFT_LOWER_LEG] = glm::length(thigh_l - calf_l);
		m_capsuleLengths[BODYPART_RIGHT_UPPER_LEG] = glm::length(thigh_r - calf_r);
		m_capsuleLengths[BODYPART_RIGHT_LOWER_LEG] = glm::length(thigh_l - calf_l);

		m_capsuleLengths[BODYPART_LEFT_UPPER_ARM] = glm::length(upperarm_l - lowerarm_l);
		m_capsuleLengths[BODYPART_LEFT_LOWER_ARM] = glm::length(hand_l - lowerarm_l);
		m_capsuleLengths[BODYPART_RIGHT_UPPER_ARM] = glm::length(upperarm_r - lowerarm_r);
		m_capsuleLengths[BODYPART_RIGHT_LOWER_ARM] = glm::length(hand_r - lowerarm_r);

		float length_upperarmL = glm::length(upperarm_l - lowerarm_l);
		float length_upperarmR = glm::length(upperarm_r - lowerarm_r);
		float length_lowerarmL = glm::length(hand_l - lowerarm_l);
		float length_lowerarmR = glm::length(hand_r - lowerarm_r);
		float length_upperlegL = glm::length(thigh_l - calf_l);
		float length_upperlegR = glm::length(thigh_r - calf_r);
		float length_lowerlegL = glm::length(calf_l - foot_l);
		float length_lowerlegR = glm::length(calf_r - foot_r);
		float length_pelvis = glm::length(pelvis - spine_01);
		//float length_spine = glm::length(spine_01 - neck_01);
		//float length_head = 0.1f;

		// Setup the geometry
		float scale_ragdoll = 0;
		float hide = 0;
		m_shapes[BODYPART_PELVIS] = new btCapsuleShape(btScalar(1 * 0.1), m_capsuleLengths[BODYPART_PELVIS]);
		m_shapes[BODYPART_SPINE_01] = new btCapsuleShape(btScalar(1 * 0.14 * 0.05), m_capsuleLengths[BODYPART_SPINE_01]);
		m_shapes[BODYPART_SPINE_02] = new btCapsuleShape(btScalar(1 * 0.16 * hide), m_capsuleLengths[BODYPART_SPINE_02]);
		m_shapes[BODYPART_SPINE_03] = new btCapsuleShape(btScalar(1 * 0.06 * hide), m_capsuleLengths[BODYPART_SPINE_03]);
		m_shapes[BODYPART_NECK] = new btCapsuleShape(btScalar(1 * 0.05 * hide), m_capsuleLengths[BODYPART_NECK]);
		m_shapes[BODYPART_HEAD] = new btCapsuleShape(btScalar(1 * 0.1 * hide), m_capsuleLengths[BODYPART_HEAD]);

		m_shapes[BODYPART_LEFT_UPPER_LEG] = new btCapsuleShape(btScalar(1 * 0.07), m_capsuleLengths[BODYPART_LEFT_UPPER_LEG]);
		m_shapes[BODYPART_LEFT_LOWER_LEG] = new btCapsuleShape(btScalar(1 * 0.05), m_capsuleLengths[BODYPART_LEFT_LOWER_LEG]);
		m_shapes[BODYPART_RIGHT_UPPER_LEG] = new btCapsuleShape(btScalar(1 * 0.07), m_capsuleLengths[BODYPART_RIGHT_UPPER_LEG]);
		m_shapes[BODYPART_RIGHT_LOWER_LEG] = new btCapsuleShape(btScalar(1 * 0.05), m_capsuleLengths[BODYPART_RIGHT_LOWER_LEG]);

		m_shapes[BODYPART_LEFT_UPPER_ARM] = new btCapsuleShape(btScalar(1 * 0.05 * hide), btScalar(length_upperarmL));
		m_shapes[BODYPART_LEFT_LOWER_ARM] = new btCapsuleShape(btScalar(1 * 0.04 * hide), btScalar(length_lowerarmL));
		m_shapes[BODYPART_RIGHT_UPPER_ARM] = new btCapsuleShape(btScalar(1 * 0.05 * hide), btScalar(length_upperarmR));
		m_shapes[BODYPART_RIGHT_LOWER_ARM] = new btCapsuleShape(btScalar(1 * 0.04 * hide), btScalar(length_lowerarmR));

		// Setup all the rigid bodies
		btTransform offset;
		offset.setIdentity();
		//offset.setOrigin(positionOffset);
	//	offset.setRotation(btQuaternion(Config::TEST_QUAT.x, Config::TEST_QUAT.y, Config::TEST_QUAT.z, Config::TEST_QUAT.w));
	//	offset.setRotation(btQuaternion(0.7071, -0.7071, 0, 0));

		btTransform transform;
		glm::mat4 boneMatrix;
		glm::quat rotate90 = glm::quat(0.7071, 0, 0, 0.7071);
		rotate90 = glm::quat(1, 0, 0, 0);
		glm::quat q;
		btQuaternion rot;



		/*transform.setIdentity();
		transform.setOrigin(Util::GetRelPosBetween2Vectors(pelvis, spine_01));
		glm::vec3 a = glm::cross(pelvis, spine_01);
		float x = a.x;
		float y = a.y;
		float z = a.z;
		float w = sqrt((glm::length(pelvis) * glm::length(pelvis)) * (glm::length(spine_01) * glm::length(spine_01))) + glm::dot(pelvis, spine_01);
		rot = btQuaternion(x, y, z, w);
		transform.setRotation(rot);
		m_bodies[BODYPART_PELVIS] = localCreateRigidBody(btScalar(1.), offset * transform, m_shapes[BODYPART_PELVIS]);
		*/


		btQuaternion rotate180(1, 0, 0, 0);

		btQuaternion rotate_X_180(1, 0, 0, 0);
		btQuaternion rotate_Y_180(0, 1, 0, 0);
		btQuaternion rotate_Y_NEG_90(0, -0.7070999742, 0, 0.7070999742);
		btQuaternion rotate_X_90(0.7070999742, 0, 0, 0.7070999742);
		btQuaternion rotate_Y_90(0, 0.7070999742, 0, 0.7070999742);
		btQuaternion rotate_Z_90(0, 0, 0.7070999742, 0.7070999742);
		btQuaternion rotate_Z_NEG_90(0, 0, 0.7070999742, -0.7070999742);
		btQuaternion magic(0.5, 0.5, 0.5, -0.5);

		btQuaternion testQuat(Config::TEST_QUAT.x, Config::TEST_QUAT.y, Config::TEST_QUAT.z, Config::TEST_QUAT.w);
		btQuaternion testQuat2(Config::TEST_QUAT2.x, Config::TEST_QUAT2.y, Config::TEST_QUAT2.z, Config::TEST_QUAT2.w);

		// PELVIS
		transform.setIdentity();
		transform.setOrigin(Util::GetRelPosBetween2Vectors(pelvis, spine_01) );
		transform.setRotation(Util::GetRotationFromBoneMatrix(boneInfo[skinnedModel->m_BoneMapping["pelvis"]].ModelSpace_AnimatedTransform)* rotate_X_90);// testQuat* testQuat2);// rotate_X_180* rotate_Y_NEG_90);
		//transform.setRotation(Util::GetRotationFromBoneMatrix(boneInfo[skinnedModel->m_BoneMapping["pelvis"]].ModelSpace_AnimatedTransform)* rotate_X_180* rotate_Y_NEG_90);
		m_bodies[BODYPART_PELVIS] = localCreateRigidBody(btScalar(1.), offset * transform, m_shapes[BODYPART_PELVIS]);


		// SPINE
		transform.setIdentity();
		transform.setOrigin(Util::GetRelPosBetween2Vectors(spine_01, spine_02));
		//transform.setRotation(Util::GetRotationFromBoneMatrix(boneInfo[skinnedModel->m_BoneMapping["spine_01"]].ModelSpace_AnimatedTransform) * testQuat* testQuat2);

		/*glm::mat4 matA = boneInfo[skinnedModel->m_BoneMapping["spine_01"]].ModelSpace_AnimatedTransform;
		glm::mat4 matB = boneInfo[skinnedModel->m_BoneMapping["spine_02"]].ModelSpace_AnimatedTransform;
		glm::quat qA = glm::quat_cast(matA);
		glm::quat qB = glm::quat_cast(matB);
		glm::quat DIFF = glm::inverse(qA) * qB;
		q = qA * DIFF;
				
		transform.setRotation(btQuaternion(q.x, q.y, q.z, q.w)* magic);//*/

		q = glm::quat_cast(boneInfo[skinnedModel->m_BoneMapping["spine_01"]].ModelSpace_AnimatedTransform);
		transform.setRotation(btQuaternion(q.x, q.y, q.z, q.w) * magic);// *btQuaternion(0.70710678118, -0.70710678118, 0, 0));
		m_bodies[BODYPART_SPINE_01] = localCreateRigidBody(btScalar(1.), offset * transform, m_shapes[BODYPART_SPINE_01]);
		

		// SPINE_02
		transform.setIdentity();
		transform.setOrigin(Util::GetRelPosBetween2Vectors(spine_02, spine_03));
		q = glm::quat_cast(boneInfo[skinnedModel->m_BoneMapping["spine_02"]].ModelSpace_AnimatedTransform);
		transform.setRotation(btQuaternion(q.x, q.y, q.z, q.w)* magic);
		m_bodies[BODYPART_SPINE_02] = localCreateRigidBody(btScalar(1.), offset * transform, m_shapes[BODYPART_SPINE_02]);

		// SPINE_03
		transform.setIdentity();
		transform.setOrigin(Util::GetRelPosBetween2Vectors(spine_03, neck_01));
		q = glm::quat_cast(boneInfo[skinnedModel->m_BoneMapping["spine_03"]].ModelSpace_AnimatedTransform);
		transform.setRotation(btQuaternion(q.x, q.y, q.z, q.w)* magic);
		m_bodies[BODYPART_SPINE_03] = localCreateRigidBody(btScalar(1.), offset * transform, m_shapes[BODYPART_SPINE_03]);


		// NECK
		transform.setIdentity();
		transform.setOrigin(Util::GetRelPosBetween2Vectors(neck_01, head)); 
		q = glm::quat_cast(boneInfo[skinnedModel->m_BoneMapping["neck_01"]].ModelSpace_AnimatedTransform);
		transform.setRotation(btQuaternion(q.x, q.y, q.z, q.w) * rotate_Z_NEG_90);
		m_bodies[BODYPART_NECK] = localCreateRigidBody(btScalar(1.), offset * transform, m_shapes[BODYPART_NECK]);
		
		// HEAD
		transform.setIdentity();
		transform.setOrigin(Util::GetRelPosBetween2Vectors(neck_01, head));
		q = glm::quat_cast(boneInfo[skinnedModel->m_BoneMapping["head"]].ModelSpace_AnimatedTransform);
		transform.setRotation(btQuaternion(q.x, q.y, q.z, q.w)* rotate_Z_NEG_90);
		m_bodies[BODYPART_HEAD] = localCreateRigidBody(btScalar(1.), offset * transform, m_shapes[BODYPART_HEAD]);


		


		transform.setIdentity();
		transform.setOrigin(Util::GetRelPosBetween2Vectors(thigh_l, calf_l));
		transform.setRotation(Util::GetRotationFromBoneMatrix(boneInfo[skinnedModel->m_BoneMapping["thigh_l"]].ModelSpace_AnimatedTransform) * rotate_X_180 * rotate_Y_NEG_90);
		m_bodies[BODYPART_LEFT_UPPER_LEG] = localCreateRigidBody(btScalar(1.), offset * transform, m_shapes[BODYPART_LEFT_UPPER_LEG]);

		transform.setIdentity();
		transform.setOrigin(Util::GetRelPosBetween2Vectors(foot_l, calf_l));
		transform.setRotation(Util::GetRotationFromBoneMatrix(boneInfo[skinnedModel->m_BoneMapping["calf_l"]].ModelSpace_AnimatedTransform) * rotate_X_180* rotate_Y_NEG_90);
		m_bodies[BODYPART_LEFT_LOWER_LEG] = localCreateRigidBody(btScalar(1.), offset * transform, m_shapes[BODYPART_LEFT_LOWER_LEG]);


		transform.setIdentity();
		transform.setOrigin(Util::GetRelPosBetween2Vectors(thigh_r, calf_r));
		transform.setRotation(Util::GetRotationFromBoneMatrix(boneInfo[skinnedModel->m_BoneMapping["thigh_r"]].ModelSpace_AnimatedTransform) * rotate_Y_90);
		m_bodies[BODYPART_RIGHT_UPPER_LEG] = localCreateRigidBody(btScalar(1.), offset * transform, m_shapes[BODYPART_RIGHT_UPPER_LEG]);

		transform.setIdentity();
		transform.setOrigin(Util::GetRelPosBetween2Vectors(foot_r, calf_r));
		transform.setRotation(Util::GetRotationFromBoneMatrix(boneInfo[skinnedModel->m_BoneMapping["calf_r"]].ModelSpace_AnimatedTransform) * rotate_Y_90);
		m_bodies[BODYPART_RIGHT_LOWER_LEG] = localCreateRigidBody(btScalar(1.), offset * transform, m_shapes[BODYPART_RIGHT_LOWER_LEG]);


		transform.setIdentity();
		transform.setOrigin(Util::GetRelPosBetween2Vectors(lowerarm_l, upperarm_l));
		transform.setRotation(Util::GetRotationFromBoneMatrix(boneInfo[skinnedModel->m_BoneMapping["upperarm_l"]].ModelSpace_AnimatedTransform)* rotate180);
		m_bodies[BODYPART_LEFT_UPPER_ARM] = localCreateRigidBody(btScalar(1.), offset * transform, m_shapes[BODYPART_LEFT_UPPER_ARM]);
		
		transform.setIdentity();
		transform.setOrigin(Util::GetRelPosBetween2Vectors(lowerarm_l, hand_l));
		transform.setRotation(Util::GetRotationFromBoneMatrix(boneInfo[skinnedModel->m_BoneMapping["lowerarm_l"]].ModelSpace_AnimatedTransform) * rotate180);
		m_bodies[BODYPART_LEFT_LOWER_ARM] = localCreateRigidBody(btScalar(1.), offset * transform, m_shapes[BODYPART_LEFT_LOWER_ARM]);


		transform.setIdentity();
		transform.setOrigin(Util::GetRelPosBetween2Vectors(lowerarm_r, hand_r));
		transform.setRotation(Util::GetRotationFromBoneMatrix(boneInfo[skinnedModel->m_BoneMapping["lowerarm_r"]].ModelSpace_AnimatedTransform)); 
		m_bodies[BODYPART_RIGHT_LOWER_ARM] = localCreateRigidBody(btScalar(1.), offset * transform, m_shapes[BODYPART_RIGHT_LOWER_ARM]);

		transform.setIdentity();
		transform.setOrigin(Util::GetRelPosBetween2Vectors(lowerarm_r, upperarm_r));
		transform.setRotation(Util::GetRotationFromBoneMatrix(boneInfo[skinnedModel->m_BoneMapping["upperarm_r"]].ModelSpace_AnimatedTransform));
		m_bodies[BODYPART_RIGHT_UPPER_ARM] = localCreateRigidBody(btScalar(1.), offset * transform, m_shapes[BODYPART_RIGHT_UPPER_ARM]);


		// Setup some damping on the m_bodies
		for (int i = 0; i < BODYPART_COUNT; ++i)
		{
			m_bodies[i]->setDamping(0.05f, 0.85f);
			m_bodies[i]->setDeactivationTime(0.8f);
			m_bodies[i]->setSleepingThresholds(1.6f, 2.5f);
			m_bodies[i]->setActivationState(0);//
			m_bodies[i]->setFriction(0.8);//
		}

		scale_ragdoll = 1;

		///////////////////////////// SETTING THE CONSTRAINTS /////////////////////////////////////////////7777
			// Now setup the constraints
		btGeneric6DofConstraint* joint6DOF;
		btTransform localA, localB;
		bool useLinearReferenceFrameA = true;

		/// ******* SPINE NECK ******** ///
		{
			localA.setIdentity();
			localA.setOrigin(btVector3(btScalar(0.), btScalar(0.30 * scale_ragdoll), btScalar(0.)));

			localB.setIdentity();
			localB.setOrigin(btVector3(btScalar(0.), btScalar(-0.14 * scale_ragdoll), btScalar(0.)));

			joint6DOF = new btGeneric6DofConstraint(*m_bodies[BODYPART_SPINE_01], *m_bodies[BODYPART_NECK], localA, localB, useLinearReferenceFrameA);
			joint6DOF->setAngularLowerLimit(btVector3(-SIMD_PI * 0.3f, -SIMD_EPSILON, -SIMD_PI * 0.3f));
			joint6DOF->setAngularUpperLimit(btVector3(SIMD_PI * 0.5f, SIMD_EPSILON, SIMD_PI * 0.3f));

			m_joints[JOINT_SPINE_NECK] = joint6DOF;
		//	m_ownerWorld->addConstraint(m_joints[JOINT_SPINE_NECK], true); 
		}
		
		/// ******* NECK HEAD ******** ///
		{
			localA.setIdentity();
			localA.setOrigin(btVector3(0, m_capsuleLengths[BODYPART_NECK] * 0.5, 0));

			localB.setIdentity();
			localB.setOrigin(btVector3(0, -m_capsuleLengths[BODYPART_NECK], 0));

			joint6DOF = new btGeneric6DofConstraint(*m_bodies[BODYPART_NECK], *m_bodies[BODYPART_HEAD], localA, localB, useLinearReferenceFrameA);
			joint6DOF->setAngularLowerLimit(btVector3(-SIMD_PI * 0.075f, -SIMD_EPSILON, -SIMD_PI * 0.075f));
			joint6DOF->setAngularUpperLimit(btVector3(SIMD_PI * 0.075f, SIMD_EPSILON, SIMD_PI * 0.075f));

			m_joints[JOINT_NECK_HEAD] = joint6DOF;
		//	m_ownerWorld->addConstraint(m_joints[JOINT_NECK_HEAD], true);
		}


		glm::vec3 Center_Of_Spine_03 = Util::btVec3_to_glmVec3(Util::GetRelPosBetween2Vectors(spine_03, neck_01));
		float Length_To_Shoulder = glm::length(Center_Of_Spine_03 - upperarm_l);

		/// ******* LEFT SHOULDER ******** ///
		{
			
			localA.setIdentity(); 
			localA.setOrigin(btVector3(-Length_To_Shoulder, 0, 0));

			localB.setIdentity();
			localB.getBasis().setEulerZYX(SIMD_HALF_PI, 0, -SIMD_HALF_PI);
			localB.setOrigin(btVector3(btScalar(0.), btScalar(-length_upperarmL * 0.5), btScalar(0.)));

			joint6DOF = new btGeneric6DofConstraint(*m_bodies[BODYPART_SPINE_03], *m_bodies[BODYPART_LEFT_UPPER_ARM], localA, localB, useLinearReferenceFrameA);

			joint6DOF->setAngularLowerLimit(btVector3(-SIMD_PI * 0.8f, -SIMD_EPSILON, -SIMD_PI * 0.5f));
			joint6DOF->setAngularUpperLimit(btVector3(SIMD_PI * 0.8f, SIMD_EPSILON, SIMD_PI * 0.5f));

			m_joints[JOINT_LEFT_SHOULDER] = joint6DOF;
		//	m_ownerWorld->addConstraint(m_joints[JOINT_LEFT_SHOULDER], true);
		}

		/// ******* RIGHT SHOULDER ******** ///
		{
			localA.setIdentity();
			localA.setOrigin(btVector3(Length_To_Shoulder, 0, 0));

			localB.setIdentity(); 
			localB.getBasis().setEulerZYX(0, 0, SIMD_HALF_PI);
			localB.setOrigin(btVector3(0, m_capsuleLengths[BODYPART_RIGHT_UPPER_ARM] * -0.5, 0));
		
			joint6DOF = new btGeneric6DofConstraint(*m_bodies[BODYPART_SPINE_03], *m_bodies[BODYPART_RIGHT_UPPER_ARM], localA, localB, useLinearReferenceFrameA);


			joint6DOF->setAngularLowerLimit(btVector3(-SIMD_PI * 0.8f, -SIMD_EPSILON, -SIMD_PI * 0.5f));
			joint6DOF->setAngularUpperLimit(btVector3(SIMD_PI * 0.8f, SIMD_EPSILON, SIMD_PI * 0.5f));

			m_joints[JOINT_RIGHT_SHOULDER] = joint6DOF;
		//	m_ownerWorld->addConstraint(m_joints[JOINT_RIGHT_SHOULDER], true);
		}

		/// ******* LEFT ELBOW ******** ///
		{;
			localA.setIdentity();
			localA.setOrigin(btVector3(btScalar(0.), btScalar(0.18 * scale_ragdoll), btScalar(0.)));

			localB.setIdentity();
			localB.setOrigin(btVector3(btScalar(0.), btScalar(-length_lowerarmL * 0.5), btScalar(0.)));

			joint6DOF = new btGeneric6DofConstraint(*m_bodies[BODYPART_LEFT_UPPER_ARM], *m_bodies[BODYPART_LEFT_LOWER_ARM], localA, localB, useLinearReferenceFrameA);
			joint6DOF->setAngularLowerLimit(btVector3(-SIMD_EPSILON, -SIMD_EPSILON, -SIMD_EPSILON));
			joint6DOF->setAngularUpperLimit(btVector3(SIMD_PI * 0.7f, SIMD_EPSILON, SIMD_EPSILON));

			m_joints[JOINT_LEFT_ELBOW] = joint6DOF;
		//	m_ownerWorld->addConstraint(m_joints[JOINT_LEFT_ELBOW], true);
		}

		/// ******* RIGHT ELBOW ******** ///
		{

		
			
			localA.setIdentity();
			localA.setOrigin(btVector3(btScalar(0.), btScalar(0.18 * scale_ragdoll), btScalar(0.)));

			localB.setIdentity();
			localB.setOrigin(btVector3(btScalar(0.), btScalar(-length_lowerarmR * 0.5), btScalar(0.)));

			joint6DOF = new btGeneric6DofConstraint(*m_bodies[BODYPART_RIGHT_UPPER_ARM], *m_bodies[BODYPART_RIGHT_LOWER_ARM], localA, localB, useLinearReferenceFrameA);

			joint6DOF->setAngularLowerLimit(btVector3(-SIMD_EPSILON, -SIMD_EPSILON, -SIMD_EPSILON));
			joint6DOF->setAngularUpperLimit(btVector3(SIMD_PI * 0.7, SIMD_EPSILON, SIMD_EPSILON));

			m_joints[JOINT_RIGHT_ELBOW] = joint6DOF;
		//	m_ownerWorld->addConstraint(m_joints[JOINT_RIGHT_ELBOW], true);
		}

		/// ******* SPINE_01 ******** ///
		{
			localA.setIdentity();
	//		localA.getBasis().setRotation(rotate_X_90);
			localA.getBasis().setEulerZYX(0, 0, SIMD_HALF_PI);
			localA.setOrigin(btVector3(0, m_capsuleLengths[BODYPART_PELVIS] * 0.5, 0));

			localB.setIdentity();
			localB.getBasis().setEulerZYX(0, SIMD_HALF_PI, 0);
			localB.setOrigin(btVector3(0, m_capsuleLengths[BODYPART_PELVIS] * -0.5, 0));

			joint6DOF = new btGeneric6DofConstraint(*m_bodies[BODYPART_PELVIS], *m_bodies[BODYPART_SPINE_01], localA, localB, useLinearReferenceFrameA);

			//joint6DOF->setAngularLowerLimit(btVector3(-SIMD_PI * 0.2, -SIMD_EPSILON, -SIMD_PI * 0.3));
			//joint6DOF->setAngularUpperLimit(btVector3(SIMD_PI * 0.2, SIMD_EPSILON, SIMD_PI * 0.6));
		
			joint6DOF->setAngularLowerLimit(btVector3(-SIMD_EPSILON, -SIMD_EPSILON, -SIMD_EPSILON));
			joint6DOF->setAngularUpperLimit(btVector3(SIMD_EPSILON, SIMD_EPSILON, SIMD_EPSILON));

			m_joints[JOINT_SPINE_01] = joint6DOF;
		//	m_ownerWorld->addConstraint(m_joints[JOINT_SPINE_01], true);
		}
		
		/// ******* SPINE_02 ******** ///
		{
			localA.setIdentity();;
			localA.setOrigin(btVector3(0, m_capsuleLengths[BODYPART_SPINE_01] * 0.5, 0));

			localB.setIdentity();
			localB.setOrigin(btVector3(0, m_capsuleLengths[BODYPART_SPINE_02] * -0.5, 0));

			joint6DOF = new btGeneric6DofConstraint(*m_bodies[BODYPART_SPINE_01], *m_bodies[BODYPART_SPINE_02], localA, localB, useLinearReferenceFrameA);

			joint6DOF->setAngularLowerLimit(btVector3(-SIMD_PI * 0.1, -SIMD_EPSILON, -SIMD_PI * 0.1));
			joint6DOF->setAngularUpperLimit(btVector3(SIMD_PI * 0.1, SIMD_EPSILON, SIMD_PI * 0.1));

			m_joints[JOINT_SPINE_02] = joint6DOF;
		//	m_ownerWorld->addConstraint(m_joints[JOINT_SPINE_02], true);
		}


		/// ******* SPINE_02 ******** ///
		{
			localA.setIdentity();;
			localA.setOrigin(btVector3(0, m_capsuleLengths[BODYPART_SPINE_02] * 0.5, 0));

			localB.setIdentity();
			localB.setOrigin(btVector3(0, m_capsuleLengths[BODYPART_SPINE_03] * -0.5, 0));

			joint6DOF = new btGeneric6DofConstraint(*m_bodies[BODYPART_SPINE_02], *m_bodies[BODYPART_SPINE_03], localA, localB, useLinearReferenceFrameA);

			joint6DOF->setAngularLowerLimit(btVector3(-SIMD_PI * 0.1, -SIMD_EPSILON, -SIMD_PI * 0.1));
			joint6DOF->setAngularUpperLimit(btVector3(SIMD_PI * 0.1, SIMD_EPSILON, SIMD_PI * 0.1));

			m_joints[JOINT_SPINE_03] = joint6DOF;
			m_ownerWorld->addConstraint(m_joints[JOINT_SPINE_03], true);
		}

		/// ******* LEFT HIP ******** ///
		{
			localA.setIdentity();
			localA.setOrigin(btVector3(glm::length(pelvis - thigh_l) * -1, m_capsuleLengths[BODYPART_PELVIS] * -0.5, 0));

			localB.setIdentity();
			localB.setOrigin(btVector3(0, m_capsuleLengths[BODYPART_LEFT_UPPER_LEG] * 0.5, 0));

			joint6DOF = new btGeneric6DofConstraint(*m_bodies[BODYPART_PELVIS], *m_bodies[BODYPART_LEFT_UPPER_LEG], localA, localB, useLinearReferenceFrameA);

		//	joint6DOF->setAngularLowerLimit(btVector3(-SIMD_HALF_PI * 0.5, -SIMD_EPSILON, -SIMD_EPSILON));
		//	joint6DOF->setAngularUpperLimit(btVector3(SIMD_HALF_PI * 0.8, SIMD_EPSILON, SIMD_HALF_PI * 0.6f));

			joint6DOF->setAngularLowerLimit(btVector3(-SIMD_EPSILON, -SIMD_EPSILON, -SIMD_EPSILON));
			joint6DOF->setAngularUpperLimit(btVector3(SIMD_EPSILON, SIMD_EPSILON, SIMD_EPSILON));


			m_joints[JOINT_LEFT_HIP] = joint6DOF;
			m_ownerWorld->addConstraint(m_joints[JOINT_LEFT_HIP], true);
		}

		/// ******* RIGHT HIP ******** ///
		{
			localA.setIdentity();
			localA.setOrigin(btVector3(glm::length(pelvis - thigh_r), m_capsuleLengths[BODYPART_PELVIS] * -0.5, 0));

			localB.setIdentity();
			localB.setOrigin(btVector3(0, m_capsuleLengths[BODYPART_RIGHT_UPPER_LEG] * 0.5, 0));

			joint6DOF = new btGeneric6DofConstraint(*m_bodies[BODYPART_PELVIS], *m_bodies[BODYPART_RIGHT_UPPER_LEG], localA, localB, useLinearReferenceFrameA);

		//	joint6DOF->setAngularLowerLimit(btVector3(-SIMD_HALF_PI * 0.5, -SIMD_EPSILON, -SIMD_HALF_PI * 0.6f));
		//	joint6DOF->setAngularUpperLimit(btVector3(SIMD_HALF_PI * 0.8, SIMD_EPSILON, SIMD_EPSILON));

			joint6DOF->setAngularLowerLimit(btVector3(-SIMD_EPSILON, -SIMD_EPSILON, -SIMD_EPSILON));
			joint6DOF->setAngularUpperLimit(btVector3(SIMD_EPSILON, SIMD_EPSILON, SIMD_EPSILON));

			m_joints[JOINT_RIGHT_HIP] = joint6DOF;
			m_ownerWorld->addConstraint(m_joints[JOINT_RIGHT_HIP], true);
		}

		/// ******* LEFT KNEE ******** ///
		{
			localA.setIdentity(); 
			localA.setOrigin(btVector3(0, m_capsuleLengths[BODYPART_LEFT_UPPER_LEG] * -0.5, 0));;

			localB.setIdentity();
			localB.setOrigin(btVector3(0, m_capsuleLengths[BODYPART_LEFT_LOWER_LEG] * 0.5, 0));
	
			joint6DOF = new btGeneric6DofConstraint(*m_bodies[BODYPART_LEFT_UPPER_LEG], *m_bodies[BODYPART_LEFT_LOWER_LEG], localA, localB, useLinearReferenceFrameA);
		//	joint6DOF->setAngularLowerLimit(btVector3(-SIMD_EPSILON, -SIMD_EPSILON, -SIMD_EPSILON));
			//joint6DOF->setAngularUpperLimit(btVector3(SIMD_PI * 0.7f, SIMD_EPSILON, SIMD_EPSILON));


			joint6DOF->setAngularLowerLimit(btVector3(-SIMD_EPSILON, -SIMD_EPSILON, -SIMD_EPSILON));
			joint6DOF->setAngularUpperLimit(btVector3(SIMD_EPSILON, SIMD_EPSILON, SIMD_EPSILON));


			m_joints[JOINT_LEFT_KNEE] = joint6DOF;
			m_ownerWorld->addConstraint(m_joints[JOINT_LEFT_KNEE], true);
		}

		/// ******* RIGHT KNEE ******** ///
		{
			localA.setIdentity();
			localA.setOrigin(btVector3(0, m_capsuleLengths[BODYPART_RIGHT_UPPER_LEG] * -0.5, 0));

			localB.setIdentity(); 
			localB.setOrigin(btVector3(0, m_capsuleLengths[BODYPART_RIGHT_LOWER_LEG] * 0.5, 0));
	
			joint6DOF = new btGeneric6DofConstraint(*m_bodies[BODYPART_RIGHT_UPPER_LEG], *m_bodies[BODYPART_RIGHT_LOWER_LEG], localA, localB, useLinearReferenceFrameA);	
		//	joint6DOF->setAngularLowerLimit(btVector3(-SIMD_EPSILON, -SIMD_EPSILON, -SIMD_EPSILON));
		//	joint6DOF->setAngularUpperLimit(btVector3(SIMD_PI * 0.7f, SIMD_EPSILON, SIMD_EPSILON));

			joint6DOF->setAngularLowerLimit(btVector3(-SIMD_EPSILON, -SIMD_EPSILON, -SIMD_EPSILON));
			joint6DOF->setAngularUpperLimit(btVector3(SIMD_EPSILON, SIMD_EPSILON, SIMD_EPSILON));

			m_joints[JOINT_RIGHT_KNEE] = joint6DOF;
			m_ownerWorld->addConstraint(m_joints[JOINT_RIGHT_KNEE], true);
		}
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

		EntityData* entityData = new EntityData();
		entityData->name = "RAGDOLL";
		body->setUserPointer(entityData);

		int group = CollisionGroups::ENEMY;
		int mask = CollisionGroups::HOUSE;// | CollisionGroups::PLAYER;


		m_ownerWorld->addRigidBody(body, group, mask);

		return body;
	}


	void Ragdoll::UpdateBoneTransform(SkinnedModel* skinnedModel, std::vector<glm::mat4>& Transforms, std::vector<glm::mat4>& DebugAnimatedTransforms)
	{
		
		// Traverse the tree 
		for (int i = 0; i < skinnedModel->m_skeleton.m_joints.size(); i++)
		{
			// Get the node and its um bind pose transform?
			const char* NodeName = skinnedModel->m_skeleton.m_joints[i].m_name;
			glm::mat4 NodeTransformation = skinnedModel->m_skeleton.m_joints[i].m_inverseBindTransform;

			
			unsigned int parentIndex = skinnedModel->m_skeleton.m_joints[i].m_parentIndex;

			glm::mat4 ParentTransformation = (parentIndex == -1) ? glm::mat4(1) : skinnedModel->m_skeleton.m_joints[parentIndex].m_currentFinalTransform;
			glm::mat4 GlobalTransformation = ParentTransformation * NodeTransformation;

			skinnedModel->m_skeleton.m_joints[i].m_currentFinalTransform = GlobalTransformation;


			if (skinnedModel->m_BoneMapping.find(NodeName) != skinnedModel->m_BoneMapping.end()) {
				unsigned int BoneIndex = skinnedModel->m_BoneMapping[NodeName];
				skinnedModel->m_BoneInfo[BoneIndex].FinalTransformation = GlobalTransformation * skinnedModel->m_BoneInfo[BoneIndex].BoneOffset;
				skinnedModel->m_BoneInfo[BoneIndex].ModelSpace_AnimatedTransform = GlobalTransformation;
							

				if (DisableSkinning) 
				{	
					glm::quat Rotate_X_180(0, 1, 0, 0);

					// PELIVS
					if (Util::StrCmp(NodeName, "pelvis")) {
						glm::mat4 matrix = Get_Bottom_Joint_World_Matrix(BODYPART_PELVIS) * glm::mat4_cast(glm::quat(0.5, 0.5, 0.5, 0.5));
						skinnedModel->m_BoneInfo[BoneIndex].FinalTransformation = matrix * skinnedModel->m_BoneInfo[BoneIndex].BoneOffset;
						skinnedModel->m_skeleton.m_joints[i].m_currentFinalTransform = matrix;
						skinnedModel->m_BoneInfo[BoneIndex].ModelSpace_AnimatedTransform = matrix;
					}

					// SPINE 01
					if (Util::StrCmp(NodeName, "spine_01")) {
						
						glm::mat4 pelvisTop = Get_Top_Joint_World_Matrix(BODYPART_PELVIS) * glm::mat4_cast(glm::quat(0.5, 0.5, 0.5, 0.5));
						glm::mat4 matrix = Get_Bottom_Joint_World_Matrix(BODYPART_SPINE_01) * glm::mat4_cast(glm::quat(0.5, 0.5, 0.5, 0.5));	

						matrix[3][0] = pelvisTop[3][0];
						matrix[3][1] = pelvisTop[3][1];
						matrix[3][2] = pelvisTop[3][2];


						skinnedModel->m_BoneInfo[BoneIndex].FinalTransformation = matrix * skinnedModel->m_BoneInfo[BoneIndex].BoneOffset;
						skinnedModel->m_skeleton.m_joints[i].m_currentFinalTransform = matrix;
						skinnedModel->m_BoneInfo[BoneIndex].ModelSpace_AnimatedTransform = matrix;
					}

					// SPINE 01
					if (Util::StrCmp(NodeName, "spine_02")) {

						//glm::mat4 spine01= Get_Top_Joint_World_Matrix(BODYPART_SPINE_01) * glm::mat4_cast(glm::quat(0.5, 0.5, 0.5, 0.5));
						glm::mat4 spine02 = Get_Bottom_Joint_World_Matrix(BODYPART_SPINE_02) * glm::mat4_cast(glm::quat(0.5, 0.5, 0.5, 0.5));

					//	spine02[3][0] = spine01[3][0];
					//	spine02[3][1] = spine01[3][1];
					//	spine02[3][2] = spine01[3][2];


						skinnedModel->m_BoneInfo[BoneIndex].FinalTransformation = spine02 * skinnedModel->m_BoneInfo[BoneIndex].BoneOffset;
						skinnedModel->m_skeleton.m_joints[i].m_currentFinalTransform = spine02;
						skinnedModel->m_BoneInfo[BoneIndex].ModelSpace_AnimatedTransform = spine02;
					}

					// SPINE 01
					if (Util::StrCmp(NodeName, "spine_03")) {
						glm::mat4 spine02 = Get_Bottom_Joint_World_Matrix(BODYPART_SPINE_03) * glm::mat4_cast(glm::quat(0.5, 0.5, 0.5, 0.5));
						skinnedModel->m_BoneInfo[BoneIndex].FinalTransformation = spine02 * skinnedModel->m_BoneInfo[BoneIndex].BoneOffset;
						skinnedModel->m_skeleton.m_joints[i].m_currentFinalTransform = spine02;
						skinnedModel->m_BoneInfo[BoneIndex].ModelSpace_AnimatedTransform = spine02;
					}

					// Head
					if (Util::StrCmp(NodeName, "neck_01")) {
						glm::mat4 matrix = Get_Bottom_Joint_World_Matrix(BODYPART_NECK) * glm::mat4_cast(glm::quat(0.7071, 0, 0, 0.7071));// glm::mat4_cast(Config::TEST_QUAT)* glm::mat4_cast(Config::TEST_QUAT2);
						skinnedModel->m_BoneInfo[BoneIndex].FinalTransformation = matrix * skinnedModel->m_BoneInfo[BoneIndex].BoneOffset;
						skinnedModel->m_skeleton.m_joints[i].m_currentFinalTransform = matrix;
					}

					// LEGS
					if (Util::StrCmp(NodeName, "thigh_r")) {
						glm::mat4 matrix = Get_Top_Joint_World_Matrix(BODYPART_RIGHT_UPPER_LEG) * glm::mat4_cast(glm::quat(-0.5, 0.5, -0.5, 0.5));
						skinnedModel->m_BoneInfo[BoneIndex].FinalTransformation = matrix * skinnedModel->m_BoneInfo[BoneIndex].BoneOffset;
						skinnedModel->m_skeleton.m_joints[i].m_currentFinalTransform = matrix;
						skinnedModel->m_BoneInfo[BoneIndex].ModelSpace_AnimatedTransform = matrix;
					}					
					if (Util::StrCmp(NodeName, "calf_r")) {
						glm::mat4 matrix = Get_Top_Joint_World_Matrix(BODYPART_RIGHT_LOWER_LEG) * glm::mat4_cast(glm::quat(-0.5, 0.5, -0.5, 0.5));
						skinnedModel->m_BoneInfo[BoneIndex].FinalTransformation = matrix * skinnedModel->m_BoneInfo[BoneIndex].BoneOffset;
						skinnedModel->m_skeleton.m_joints[i].m_currentFinalTransform = matrix;
						skinnedModel->m_BoneInfo[BoneIndex].ModelSpace_AnimatedTransform = matrix;
					}					
					if (Util::StrCmp(NodeName, "thigh_l")) {
						glm::mat4 matrix = Get_Top_Joint_World_Matrix(BODYPART_LEFT_UPPER_LEG) * glm::mat4_cast(glm::quat(0.5, 0.5, 0.5, 0.5));
						skinnedModel->m_BoneInfo[BoneIndex].FinalTransformation = matrix * skinnedModel->m_BoneInfo[BoneIndex].BoneOffset;
						skinnedModel->m_skeleton.m_joints[i].m_currentFinalTransform = matrix;
						skinnedModel->m_BoneInfo[BoneIndex].ModelSpace_AnimatedTransform = matrix;
					}				
					if (Util::StrCmp(NodeName, "calf_l"))
					{
						glm::mat4 matrix = Get_Top_Joint_World_Matrix(BODYPART_LEFT_LOWER_LEG) * glm::mat4_cast(glm::quat(0.5, 0.5, 0.5, 0.5));
						skinnedModel->m_BoneInfo[BoneIndex].FinalTransformation = matrix * skinnedModel->m_BoneInfo[BoneIndex].BoneOffset;
						skinnedModel->m_skeleton.m_joints[i].m_currentFinalTransform = matrix;
						skinnedModel->m_BoneInfo[BoneIndex].ModelSpace_AnimatedTransform = matrix;
					}

					
					// ARMS
					if (Util::StrCmp(NodeName, "upperarm_r"))
					{
						glm::mat4 matrix = Get_Bottom_Joint_World_Matrix(BODYPART_RIGHT_UPPER_ARM) * glm::mat4_cast(glm::quat(0.7071, 0, 0, -0.7071) * Rotate_X_180);
						skinnedModel->m_BoneInfo[BoneIndex].FinalTransformation = matrix * skinnedModel->m_BoneInfo[BoneIndex].BoneOffset;
						skinnedModel->m_skeleton.m_joints[i].m_currentFinalTransform = matrix;
						skinnedModel->m_BoneInfo[BoneIndex].ModelSpace_AnimatedTransform = matrix;
					}
					if (Util::StrCmp(NodeName, "lowerarm_r"))
					{
						glm::mat4 matrix = Get_Bottom_Joint_World_Matrix(BODYPART_RIGHT_LOWER_ARM) * glm::mat4_cast(glm::quat(0.7071, 0, 0, -0.7071) * Rotate_X_180);
						skinnedModel->m_BoneInfo[BoneIndex].FinalTransformation = matrix * skinnedModel->m_BoneInfo[BoneIndex].BoneOffset;
						skinnedModel->m_skeleton.m_joints[i].m_currentFinalTransform = matrix;
					}


					if (Util::StrCmp(NodeName, "upperarm_l"))
					{
						glm::mat4 matrix = Get_Bottom_Joint_World_Matrix(BODYPART_LEFT_UPPER_ARM) * glm::mat4_cast(glm::quat(0.7071, 0, 0, 0.7071));// glm::mat4_cast(Config::TEST_QUAT) * glm::mat4_cast(Config::TEST_QUAT2);
						skinnedModel->m_BoneInfo[BoneIndex].FinalTransformation = matrix * skinnedModel->m_BoneInfo[BoneIndex].BoneOffset;
						skinnedModel->m_skeleton.m_joints[i].m_currentFinalTransform = matrix;
						skinnedModel->m_BoneInfo[BoneIndex].ModelSpace_AnimatedTransform = matrix;
					}
					if (Util::StrCmp(NodeName, "lowerarm_l"))
					{
						glm::mat4 matrix = Get_Bottom_Joint_World_Matrix(BODYPART_LEFT_LOWER_ARM) * glm::mat4_cast(glm::quat(0.7071, 0, 0, 0.7071));// glm::mat4_cast(Config::TEST_QUAT)* glm::mat4_cast(Config::TEST_QUAT2);
						skinnedModel->m_BoneInfo[BoneIndex].FinalTransformation = matrix * skinnedModel->m_BoneInfo[BoneIndex].BoneOffset;
						skinnedModel->m_skeleton.m_joints[i].m_currentFinalTransform = matrix;
					}

				}
				
			}

			//  * glm::mat4_cast(Config::TEST_QUAT) * glm::mat4_cast(Config::TEST_QUAT2)
			
		}

		for (unsigned int i = 0; i < skinnedModel->m_NumBones; i++) {
			Transforms[i] = skinnedModel->m_BoneInfo[i].FinalTransformation;
			DebugAnimatedTransforms[i] = skinnedModel->m_BoneInfo[i].ModelSpace_AnimatedTransform;
		}
		
		/*

		// Climb down the tree and build the transforms. You retreuve them just below see, from m_BoneInfo.
		ReadNodeHeirarchy(skinnedModel, skinnedModel->m_pScene->mRootNode, glm::mat4(1));

		for (unsigned int i = 0; i < skinnedModel->m_NumBones; i++) {
			Transforms[i] = skinnedModel->m_BoneInfo[i].FinalTransformation;
			DebugAnimatedTransforms[i] = skinnedModel->m_BoneInfo[i].DebugMatrix_AnimatedTransform;
		}*/
	}

	glm::vec3 Ragdoll::GetJointWorldPosition(int jointIndex)
	{
		if (jointIndex < 0 || jointIndex >= JOINT_COUNT)
			return glm::vec3(0);

		btGeneric6DofConstraint* constraint = m_joints[jointIndex];
		constraint->calculateTransforms();

		btTransform transform = constraint->getCalculatedTransformB();
		btVector3 pos = transform.getOrigin();
		return Util::btVec3_to_glmVec3(pos);
	}

	glm::vec3 Ragdoll::GetJointWorldPositionA(int jointIndex)
	{
		// ok so this is actually the world position offset for the joint from the main ragdoll i think. defs not the pivot.
		if (jointIndex < 0 || jointIndex >= JOINT_COUNT)
			return glm::vec3(0);

		btGeneric6DofConstraint* constraint = m_joints[jointIndex];
		constraint->calculateTransforms();

		btTransform transform = constraint->getCalculatedTransformA();
		btVector3 pos = transform.getOrigin();
		return Util::btVec3_to_glmVec3(pos);
	}

	glm::mat4 Ragdoll::Get_Top_Joint_World_Matrix(int BodyPartIndex)
	{
		btTransform worldTransform;
		m_bodies[BodyPartIndex]->getMotionState()->getWorldTransform(worldTransform);
		Transform localOffsetTransform;
		localOffsetTransform.position.y = m_capsuleLengths[BodyPartIndex] * 0.5;
		return Util::btTransformToMat4(worldTransform) * localOffsetTransform.to_mat4();
	}

	glm::mat4 Ragdoll::Get_Bottom_Joint_World_Matrix(int BodyPartIndex)
	{
		btTransform worldTransform;
		m_bodies[BodyPartIndex]->getMotionState()->getWorldTransform(worldTransform);
		Transform localOffsetTransform;
		localOffsetTransform.position.y = m_capsuleLengths[BodyPartIndex] * -0.5;
		return Util::btTransformToMat4(worldTransform) * localOffsetTransform.to_mat4();
	}

	/*glm::mat4 Ragdoll::GetJointWorldMatrix(int jointIndex)
	{
		if (jointIndex < 0 || jointIndex >= JOINT_COUNT)
			return glm::mat4(1);

		//glm::vec3 position = GetJointWorldPosition(jointIndex);


		
		btGeneric6DofConstraint* constraint = m_joints[jointIndex];
		constraint->calculateTransforms();
		btTransform transform = constraint->getCalculatedTransformB();
		btTransform transform2 = constraint->getCalculatedTransformA();

		btVector3 pos = transform.getOrigin();
		btQuaternion rot = btQuaternion(Config::TEST_QUAT.x, Config::TEST_QUAT.y, Config::TEST_QUAT.z, Config::TEST_QUAT.w) * transform2.getRotation();

		glm::vec3 posGL = glm::vec3(pos.x(), pos.y(), pos.z());
		glm::quat rotQL = glm::quat(rot.w(), rot.x(), rot.y(), rot.z());

		glm::mat4 m = glm::translate(glm::mat4(1), posGL);
		m *= glm::mat4_cast(rotQL);
		return m;
	}*/
}