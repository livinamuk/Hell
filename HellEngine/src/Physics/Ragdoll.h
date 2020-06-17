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

namespace HellEngine
{
	class Ragdoll
	{
	public:
		enum
		{
			BODYPART_PELVIS = 0,
			BODYPART_SPINE,
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

		enum
		{
			JOINT_PELVIS_SPINE = 0,
			JOINT_SPINE_HEAD,

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
		btCollisionShape* m_shapes[BODYPART_COUNT];
		btTypedConstraint* m_joints[JOINT_COUNT];

		btRigidBody* localCreateRigidBody(btScalar mass, const btTransform& startTransform, btCollisionShape* shape);
	
	public:
		Ragdoll();
		Ragdoll(const btVector3& positionOffset, float modelScale, glm::mat4 worldMatrix);

		~Ragdoll();

		btRigidBody* m_bodies[BODYPART_COUNT];
	};
}