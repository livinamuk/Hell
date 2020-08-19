#pragma once
#include "Header.h"
#include "bullet/src/btBulletCollisionCommon.h"

namespace HellEngine
{
	class Window
	{

	public: // methods
		Window(glm::vec3 position, Axis axis);
		Window(const Window&);
		Window& operator = (const Window& input);

		void DrawFrameAndSashes(Shader* shader);
		void DrawGlass(Shader* shader);
		void Reconfigure();
		void CreateCollisionObject(); 
		void RemoveCollisionObject();

	public: // members
		Transform m_transform;
		Axis m_axis;
		btCollisionObject* m_collisionObject; // for mouse picking in the level editor
	};
}