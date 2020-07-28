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
		m_collisionObject_UpperSegment = Physics::AddWallSegment(transform);

		// Create lower collision object
		if (heightOfLowerSegment > 0) {
			Transform transform;
			transform.position = position;
			transform.position.y -= (ROOM_HEIGHT / 2) - (heightOfLowerSegment / 2);
			transform.rotation = Util::SetRotationByAxis(m_axis);
			transform.scale = glm::vec3(width, heightOfLowerSegment, WALL_DEPTH);
			m_collisionObject_LowerSegment = Physics::AddWallSegment(transform);
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
}