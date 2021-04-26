#pragma once

#include "header.h"
#include "bullet/src/LinearMath/btAlignedObjectArray.h"
#include "bullet/src/btBulletDynamicsCommon.h"

#include "bullet/src/btBulletCollisionCommon.h"
#include "bullet/src/LinearMath/btVector3.h"


#include "bullet/src/btBulletCollisionCommon.h"
#include "bullet/src/btBulletDynamicsCommon.h"
#include "bullet/src/LinearMath/btVector3.h"
#include "bullet/src/LinearMath/btAlignedObjectArray.h"
#include "bullet/src/BulletCollision/CollisionDispatch/btGhostObject.h"
#include "bullet/src/BulletDynamics/Character/btKinematicCharacterController.h"


#include <glm/gtx/quaternion.hpp>
#include "Animation/SkinnedModel.h"

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include<assimp/postprocess.h>     // Post processing flags

namespace HellEngine
{
	class Ragdoll
	{
	public:

		float DIST_FROM_THIGH_L_TO_THIGH_R;
		float DIST_FROM_PELVIS_TO_SPINE_01;
		float DIST_FROM_SPINE_01_TO_SPINE_02;
		float DIST_FROM_SPINE_02_TO_NECK;
		float DIST_FROM_SHOULDER_L_TO_SHOULDER_R;
		float DIST_FROM_SHOULDER_TO_ELBOW;
		float DIST_VERTICALLY_FROM_SHOULDER_TO_SPINE_02;
		float DIST_FROM_ELBO_TO_HAND;
		float DIST_FROM_NECK_TO_HEAD;
		float HEAD_SPHERE_RADIUS;

		enum
		{
			BODYPART_PELVIS = 0,

			BODYPART_LOWER_TORSO,
			BODYPART_UPPER_TORSO,

			BODYPART_NECK,
			BODYPART_HEAD,

			BODYPART_LEFT_UPPER_LEG,
			BODYPART_LEFT_LOWER_LEG,

			BODYPART_RIGHT_UPPER_LEG,
			BODYPART_RIGHT_LOWER_LEG,

			BODYPART_LEFT_UPPER_ARM,
			BODYPART_LEFT_LOWER_ARM,

			BODYPART_RIGHT_UPPER_ARM,
			BODYPART_RIGHT_LOWER_ARM,

			BODYPART_COUNT
		};

		enum RAGDOLL_JOINT
		{
			JOINT_SPINE_01 = 0,
			JOINT_SPINE_02,

			JOINT_SPINE_NECK,
			JOINT_HEAD,

			JOINT_LEFT_HIP,
			JOINT_LEFT_KNEE,

			JOINT_RIGHT_HIP,
			JOINT_RIGHT_KNEE,

			JOINT_LEFT_SHOULDER,
			JOINT_LEFT_ELBOW,

			JOINT_RIGHT_SHOULDER,
			JOINT_RIGHT_ELBOW,

			JOINT_COUNT
		};

		btDynamicsWorld* m_ownerWorld;

		float m_capsuleHeights[BODYPART_COUNT];
		float m_capsuleWidths[BODYPART_COUNT];
		btCollisionShape* m_shapes[BODYPART_COUNT];
		//btTypedConstraint* m_joints[JOINT_COUNT];
		btGeneric6DofConstraint* m_joints[JOINT_COUNT];

		btRigidBody* localCreateRigidBody(btScalar mass, const btTransform& startTransform, btCollisionShape* shape);
	
	public:
		bool DisableSkinning;

		Ragdoll();
		Ragdoll(const btVector3& positionOffset, float modelScale, glm::mat4 worldMatrix);
		~Ragdoll();

		void UpdateBoneTransform2(SkinnedModel* skinnedModel, std::vector<glm::mat4>& Transforms, std::vector<glm::mat4>& DebugAnimatedTransforms);

		btRigidBody* m_bodies[BODYPART_COUNT];

		Transform* p_worldTransform;
		Transform* p_modelTransform;

		//glm::mat4 GetJointWorldMatrix(int jointIndex);
		glm::vec3 GetJointWorldPosition(int jointIndex);
		glm::vec3 GetJointWorldPositionA(int jointIndex);


		glm::mat4 Get_Top_Joint_World_Matrix(int BodyPartIndex);
		glm::mat4 Get_Bottom_Joint_World_Matrix(int BodyPartIndex);
	};
}