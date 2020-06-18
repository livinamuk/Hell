#include "hellpch.h"
#include "RagdollJoint.h"
#include "Helpers/Util.h"
#include "Physics.h"
//#include <math.h>

namespace HellEngine
{

/*	RagdollJoint::RagdollJoint()
	{
		btTransform localA, localB;
		bool useLinearReferenceFrameA = true;

		localA.setIdentity(); localB.setIdentity();

		localA.setOrigin(btVector3(btScalar(0.), btScalar(0.30), btScalar(0.)));

		localB.setOrigin(btVector3(btScalar(0.), btScalar(-0.14), btScalar(0.)));

		m_constraint = new btGeneric6DofConstraint(*m_bodies[BODYPART_SPINE], *m_bodies[BODYPART_HEAD], localA, localB, useLinearReferenceFrameA);
		m_constraint->setAngularLowerLimit(btVector3(-SIMD_PI * 0.3f, -SIMD_EPSILON, -SIMD_PI * 0.3f));
		m_constraint->setAngularUpperLimit(btVector3(SIMD_PI * 0.5f, SIMD_EPSILON, SIMD_PI * 0.3f));

		Physics::s_dynamicsWorld->addConstraint(m_joints[JOINT_SPINE_HEAD], true);
	}

	glm::vec3 RagdollJoint::GetWorldPosition()
	{
		m_constraint->calculateTransforms();
		btTransform transform = m_constraint->getCalculatedTransformB();
		glm::vec3 worldPosition = Util::btVec3_to_glmVec3(transform.getOrigin());
		return worldPosition;
	}*/
}