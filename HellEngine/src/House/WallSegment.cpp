#include "hellpch.h"
#include "WallSegment.h"
#include "Physics/Physics.h"
#include "Helpers/Util.h"

namespace HellEngine
{
	WallSegment::WallSegment(glm::vec3 position, Axis axis, float width, float heightOfUpperSegment, float heightOfLowerSegment)
	{
		m_position = position;
		m_axis = axis;
		m_width = width;
		m_heightOfUpperSegment = heightOfUpperSegment;
		m_heightOfLowerSegment = heightOfLowerSegment;

		// Create upper collision object
		Transform transform;
		transform.position = position;
		transform.position.y += (ROOM_HEIGHT / 2) - (heightOfUpperSegment / 2);
		transform.rotation = Util::SetRotationByAxis(m_axis);
		transform.scale = glm::vec3(width, heightOfUpperSegment, WALL_DEPTH);
		CreateUpperCollisionObject_UpperSegment(transform);

		// Create lower collision object
		if (heightOfLowerSegment > 0) {
			Transform transform;
			transform.position = position;
			transform.position.y -= (ROOM_HEIGHT / 2) - (heightOfLowerSegment / 2);
			transform.rotation = Util::SetRotationByAxis(m_axis);
			transform.scale = glm::vec3(width, heightOfLowerSegment, WALL_DEPTH);
			CreateUpperCollisionObject_LowerSegment(transform);
		}
	}

	WallSegment::~WallSegment()
	{
	}

	void WallSegment::RemoveCollisionObjects()
	{	// Remove upper collision object
		Physics::s_dynamicsWorld->removeCollisionObject(m_collisionObject_UpperSegment);

		// Remove lower collision object
		if (m_heightOfLowerSegment > 0)
			Physics::s_dynamicsWorld->removeCollisionObject(m_collisionObject_LowerSegment);
	}

	glm::vec3 WallSegment::GetBottomLeftWorldPosCorner()
	{
		Transform trans;
		trans.position = m_position;
		trans.rotation = Util::SetRotationByAxis(m_axis);

		glm::mat4 m = trans.to_mat4();
		glm::vec4 v = m * glm::vec4(-m_width / 2, -ROOM_HEIGHT / 2, 0.0f, 1.0f);

		return glm::vec3(v.x, v.y, v.z);
	}

	glm::vec3 WallSegment::GetTopRightWorldPosCorner()
	{
		Transform trans;
		trans.position = m_position;
		trans.rotation = Util::SetRotationByAxis(m_axis);
		
		glm::mat4 m = trans.to_mat4();
		glm::vec4 v = m * glm::vec4(m_width / 2, ROOM_HEIGHT/2, 0.0f, 1.0f);

		return glm::vec3(v.x, v.y, v.z);
	}

	void WallSegment::CreateUpperCollisionObject_UpperSegment(const Transform& trans)
	{
		static bool hasBeenCreatedBefore = false;
		if (hasBeenCreatedBefore) {
			Physics::s_dynamicsWorld->removeCollisionObject(m_collisionObject_UpperSegment);
			delete m_collisionShape_UpperSegment;
			delete m_collisionObject_UpperSegment;
			hasBeenCreatedBefore = true;
		}

		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(Util::glmVec3_to_btVec3(trans.position));
		transform.setRotation(Util::glmVec3_to_btQuat(trans.rotation));

		btTransform depthOffsetTransform;
		depthOffsetTransform.setIdentity();
		depthOffsetTransform.setOrigin(btVector3(0, 0, -WALL_DEPTH / 2));

		float friction = 0.5f;

		int collisionGroup = CollisionGroups::HOUSE;
		int collisionMask = CollisionGroups::PLAYER | CollisionGroups::PROJECTILES | CollisionGroups::ENEMY;
		m_collisionShape_UpperSegment = new btBoxShape(btVector3(0.5, 0.5, 0.5));
		m_collisionShape_UpperSegment->setLocalScaling(Util::glmVec3_to_btVec3(trans.scale));
		PhysicsObjectType objectType = PhysicsObjectType::WALL;

		m_collisionObject_UpperSegment = Physics::CreateCollisionObject(transform * depthOffsetTransform, m_collisionShape_UpperSegment, objectType, collisionGroup, collisionMask, friction, DEBUG_COLOR_WALL, this);
	}

	void WallSegment::CreateUpperCollisionObject_LowerSegment(const Transform& trans)
	{
		static bool hasBeenCreatedBefore = false;
		if (hasBeenCreatedBefore) {
			Physics::s_dynamicsWorld->removeCollisionObject(m_collisionObject_LowerSegment);
			delete m_collisionShape_LowerSegment;
			delete m_collisionObject_LowerSegment;
			hasBeenCreatedBefore = true;
		}

		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(Util::glmVec3_to_btVec3(trans.position));
		transform.setRotation(Util::glmVec3_to_btQuat(trans.rotation));

		btTransform depthOffsetTransform;
		depthOffsetTransform.setIdentity();
		depthOffsetTransform.setOrigin(btVector3(0, 0, -WALL_DEPTH / 2));

		float friction = 0.5f;

		int collisionGroup = CollisionGroups::HOUSE;
		int collisionMask = CollisionGroups::PLAYER | CollisionGroups::PROJECTILES | CollisionGroups::ENEMY;
		m_collisionShape_LowerSegment = new btBoxShape(btVector3(0.5, 0.5, 0.5));
		m_collisionShape_LowerSegment->setLocalScaling(Util::glmVec3_to_btVec3(trans.scale));
		PhysicsObjectType objectType = PhysicsObjectType::WALL;

		m_collisionObject_LowerSegment = Physics::CreateCollisionObject(transform * depthOffsetTransform, m_collisionShape_LowerSegment, objectType, collisionGroup, collisionMask, friction, DEBUG_COLOR_WALL, this);

	}
}