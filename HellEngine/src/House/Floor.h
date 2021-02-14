#pragma once
#include "Header.h"
#include "bullet/src/btBulletCollisionCommon.h"

namespace HellEngine
{
	class Floor
	{
	public: // methods
		Floor();
	//	Floor(glm::vec3 position, glm::vec2 size, int story, bool rotateTexture, void* parent);
		Floor(Transform transform, bool rotateTexture, void* parent);
		//Floor(glm::vec3 position, glm::vec2 size); // for stairs
		void Draw(Shader* shader);
		void CalculateWorldSpaceCorners();
		void CreateCollisionObject();
		void RemoveCollisionObject();

	public:	// fields
		Transform m_transform; 
		bool m_rotateTexture;
		void* m_parent;
		std::vector<glm::vec3>worldSpaceCorners;
		btCollisionObject* m_collisionObject;
		btCollisionShape* m_collisionShape;

		GLuint m_decalMapID;
	};
}