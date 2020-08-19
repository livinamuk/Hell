#pragma once
#include "Header.h"
#include "bullet/src/btBulletCollisionCommon.h"

namespace HellEngine
{
	class Entity
	{
	public: // methods
		Entity();
		Entity(const Entity&);
		Entity& operator = (const Entity& input);


		Entity(char* name);
		Entity(int modelID);
		void Update(float deltaTime);
		void DrawEntity(Shader* shader);
		void Draw(Shader* shader);
		void Draw(Shader* shader, glm::mat4 modelMatrix);
		void SetModelID(int modelID);
		int GetModelID();
		void UpdateCollisionObject();
		void RemoveCollisionObject();

	public:	// fields
		Transform m_transform;
		int m_materialID = -1;
		char* m_tag = "Entity";
		btCollisionObject* m_collisionObject;
		btTriangleMesh* m_triangleMesh;
		btBvhTriangleMeshShape* m_triangleMeshShape;

	private: // field
		int m_modelID = -1;
	};
}