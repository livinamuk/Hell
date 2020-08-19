#include "hellpch.h"
#include "Ceiling.h"
#include "Helpers/Util.h"
#include "Helpers/AssetManager.h"
#include "GL/GpuProfiling.h"
#include "Physics/Physics.h"

namespace HellEngine
{
	Ceiling::Ceiling()
	{
	}

	Ceiling::Ceiling(Transform transform, bool rotateTexture, void* parent)
	{
		m_transform = transform;
		m_rotateTexture = rotateTexture;
		m_parent = parent;
		CalculateWorldSpaceCorners();
		CreateCollisionObject();
	}

/*	Ceiling::Ceiling(glm::vec3 position, glm::vec2 size, int story, bool rotateTexture)
	{
		m_transform.position.x = position.x;
		m_transform.position.y = ROOM_HEIGHT + (STORY_HEIGHT * story);
		m_transform.position.z = position.z;
		m_transform.scale.x = size.x;
		m_transform.scale.y = 1;
		m_transform.scale.z = size.y;
		m_rotateTexture = rotateTexture;
	}*/

	/*Ceiling::Ceiling(glm::vec3 position, glm::vec2 size) // for stairs
	{
		m_transform.position = position;
		m_transform.scale.x = size.x;
		m_transform.scale.z = size.y;
		CalculateWorldSpaceCorners();
	}*/

	void Ceiling::Draw(Shader* shader)
	{
		GpuProfiler g("Ceiling");
		if (m_rotateTexture)
			shader->setInt("TEXTURE_FLAG", 3);
		else
			shader->setInt("TEXTURE_FLAG", 4);

		Util::DrawDownFacingPlane(shader, &m_transform);
		shader->setInt("TEXTURE_FLAG", 0);
	}

	void Ceiling::CalculateWorldSpaceCorners()
	{
		glm::vec3 a = m_transform.to_mat4() * glm::vec4(glm::vec3(-0.5f, 0, 0.5f), 1.0f);
		glm::vec3 b = m_transform.to_mat4() * glm::vec4(glm::vec3(0.5f, 0, 0.5f), 1.0f);
		glm::vec3 c = m_transform.to_mat4() * glm::vec4(glm::vec3(0.5f, 0, -0.5f), 1.0f);
		glm::vec3 d = m_transform.to_mat4() * glm::vec4(glm::vec3(-0.5f, 0, -0.5f), 1.0f);

		worldSpaceCorners.clear();
		worldSpaceCorners.push_back(glm::vec3(a));
		worldSpaceCorners.push_back(glm::vec3(b));
		worldSpaceCorners.push_back(glm::vec3(c));
		worldSpaceCorners.push_back(glm::vec3(d));
	}

	void Ceiling::CreateCollisionObject()
	{
		//static bool hasBeenCreatedBefore = false;
		///if (hasBeenCreatedBefore) {
		//	Physics::s_dynamicsWorld->removeCollisionObject(m_collisionObject);
			//delete m_collisionShape;
			//delete m_collisionObject;
	//	}
		//hasBeenCreatedBefore = true;

		glm::vec3 position = m_transform.position;
		position.y += FLOOR_THICKNESS / 2;

		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(Util::glmVec3_to_btVec3(position));
		transform.setRotation(Util::glmVec3_to_btQuat(m_transform.rotation));

		float friction = 0.8f;
		int collisionGroup = CollisionGroups::HOUSE;
		int collisionMask = CollisionGroups::PLAYER | CollisionGroups::PROJECTILES;
		m_collisionShape = new btBoxShape(btVector3(0.5, 0.5, 0.5));
		m_collisionShape->setLocalScaling(btVector3(m_transform.scale.x, CEILING_THICKNESS, m_transform.scale.z));
		PhysicsObjectType objectType = PhysicsObjectType::UNDEFINED;

		m_collisionObject = Physics::CreateCollisionObject(transform, m_collisionShape, objectType, collisionGroup, collisionMask, friction, DEBUG_COLOR_WALL, this);
	}

	void Ceiling::RemoveCollisionObject()
	{
		Physics::s_dynamicsWorld->removeCollisionObject(m_collisionObject);
	}
}