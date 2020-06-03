#include "hellpch.h"
#include "RaycastResult.h"

namespace HellEngine
{
	RaycastResult::RaycastResult()
	{

	}

	void RaycastResult::CastRay(glm::vec3 rayOrigin, glm::vec3 rayDirection, float rayLength, float variance)
	{
		// Bail if NAN origin`
		if (rayOrigin.x != rayOrigin.x)
			return;

		// Reset
		m_elementIndex = -1;
		m_distance = 0;
		m_hitPoint = glm::vec3(0);;
		m_surfaceNormal = glm::vec3(0);
		m_name = "UNKNOWN";
		
		// Variance
		float offset = (variance * 0.5f) - Util::RandomFloat(0, variance);
		rayDirection.x += offset;
		offset = (variance * 0.5f) - Util::RandomFloat(0, variance);
		rayDirection.y += offset;
		offset = (variance * 0.5f) - Util::RandomFloat(0, variance);
		rayDirection.z += offset;
		rayDirection = glm::normalize(rayDirection);

		// End of ray
		glm::vec3 rayEnd = rayOrigin + (rayDirection * glm::vec3(rayLength));

		btCollisionWorld::ClosestRayResultCallback rayCallback(
			btVector3(rayOrigin.x, rayOrigin.y, rayOrigin.z),
			btVector3(rayEnd.x, rayEnd.y, rayEnd.z)
		);

		// Ingore player capsule
		//rayCallback.m_collisionFilterMask &= ~btBroadphaseProxy::CharacterFilter;
		//rayCallback.m_collisionFilterMask &= CollisionGroups::HOUSE | CollisionGroups::PROJECTILES;
		rayCallback.m_collisionFilterGroup = btBroadphaseProxy::AllFilter;
		rayCallback.m_collisionFilterMask =  ~CollisionGroups::PLAYER;

		Physics::s_dynamicsWorld->rayTest(
			btVector3(rayOrigin.x, rayOrigin.y, rayOrigin.z),
			btVector3(rayEnd.x, rayEnd.y, rayEnd.z),
			rayCallback
		);

		if (rayCallback.hasHit())
		{
			// Collision object
			btVector3 objectCOM = rayCallback.m_collisionObject->getWorldTransform().getOrigin();
			btVector3 RayCastOffsetFromCOM = objectCOM - rayCallback.m_hitPointWorld;

			// Find rigid body
			int RayCastWorldArrayIndex = (int)rayCallback.m_collisionObject->getWorldArrayIndex();

			btRigidBody* rigidBody = (btRigidBody*)rayCallback.m_collisionObject;

			m_hitPoint = Util::btVec3_to_glmVec3(rayCallback.m_hitPointWorld);
			m_distance = (rayCallback.m_hitPointWorld - Util::glmVec3_to_btVec3(rayOrigin)).length();
			m_surfaceNormal = Util::btVec3_to_glmVec3(rayCallback.m_hitNormalWorld);

			EntityData* entityData = (EntityData*)rigidBody->getUserPointer();
			if (entityData) {
				m_elementIndex = entityData->vectorIndex;
				m_name = entityData->name;
			}
		}
	}
}