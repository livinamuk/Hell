#pragma once
#include "Header.h"
#include "bullet/src/btBulletCollisionCommon.h"

namespace HellEngine
{
	class Ceiling
	{
	public: // methods
		Ceiling();
		Ceiling(Transform transform, bool rotateTexture, void* parent);
		//Ceiling(glm::vec3 position, glm::vec2 size, int story, bool rotateTexture);
		//Ceiling(glm::vec3 position, glm::vec2 size);
		void Draw(Shader* shader);
		void CalculateWorldSpaceCorners();
		void CreateCollisionObject();
		void RemoveCollisionObject();

	public:	// fields
		Transform m_transform;
		bool m_rotateTexture;
		std::vector<glm::vec3>worldSpaceCorners;
		btCollisionObject* m_collisionObject;
		btCollisionShape* m_collisionShape;
		void* m_parent;
	};
}