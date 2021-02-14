#include "hellpch.h"
#include "Floor.h"
#include "Helpers/Util.h"
#include "Helpers/AssetManager.h"
#include "GL/GpuProfiling.h"
#include "Physics/Physics.h"

namespace HellEngine
{
	Floor::Floor()
	{
	}

	Floor::Floor(Transform transform, bool rotateTexture, void* parent)
	{
		m_transform = transform;
		m_rotateTexture = rotateTexture;
		m_parent = parent;
		CalculateWorldSpaceCorners();
		CreateCollisionObject();

		glGenTextures(1, &m_decalMapID);
		glBindTexture(GL_TEXTURE_2D, m_decalMapID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);

		float texScale = 100;

		// Generate texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (int)(m_transform.scale.x * texScale), (int)(m_transform.scale.y * texScale), 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		glGenerateMipmap(GL_TEXTURE_2D);

		std::cout << "FLOOR: " << m_decalMapID  << " " << m_transform.scale.x << " x " << m_transform.scale.z << " TEXTURE: " << (int)(m_transform.scale.x * texScale) << " x " << (int)(m_transform.scale.z * texScale) << "\n";
	}

	/*Floor::Floor(glm::vec3 position, glm::vec2 size) // for stairs
	{
		m_transform.position = position;
		m_transform.scale.x = size.x;
		m_transform.scale.z = size.y;
		CalculateWorldSpaceCorners();
	}*/

	void Floor::Draw(Shader* shader)
	{
		GpuProfiler g("Floor");
		if (m_rotateTexture)
			shader->setInt("TEXTURE_FLAG", 3);
		else
			shader->setInt("TEXTURE_FLAG", 4);

		Util::DrawUpFacingPlane(shader, &m_transform);
		shader->setInt("TEXTURE_FLAG", 0);
	}

	void Floor::CalculateWorldSpaceCorners()
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

	void Floor::CreateCollisionObject()
	{
		/*static bool hasBeenCreatedBefore = false;
		if (hasBeenCreatedBefore) {
			Physics::s_dynamicsWorld->removeCollisionObject(m_collisionObject);
			//delete m_collisionShape;
			delete m_collisionObject;
			hasBeenCreatedBefore = true;
		}*/

		glm::vec3 position = m_transform.position;
		position.y -= FLOOR_THICKNESS / 2;

		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(Util::glmVec3_to_btVec3(position));
		transform.setRotation(Util::glmVec3_to_btQuat(m_transform.rotation));

		float friction = 0.8f;			
		int collisionGroup = CollisionGroups::HOUSE;
		int collisionMask = CollisionGroups::PLAYER | CollisionGroups::PROJECTILES | CollisionGroups::ENEMY;
	
		// STATIC collision shape
		m_collisionShape = new btBoxShape(btVector3(0.5, 0.5, 0.5)); 
		m_collisionShape->setLocalScaling(btVector3(m_transform.scale.x, FLOOR_THICKNESS, m_transform.scale.z));

		PhysicsObjectType objectType = PhysicsObjectType::FLOOR;

		m_collisionObject = Physics::CreateCollisionObject(transform, m_collisionShape, objectType, collisionGroup, collisionMask, friction, DEBUG_COLOR_YELLOW, this);
	}

	void Floor::RemoveCollisionObject()
	{
		Physics::s_dynamicsWorld->removeCollisionObject(m_collisionObject);
	}
}