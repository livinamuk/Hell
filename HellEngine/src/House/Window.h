#pragma once
#include "Header.h"
#include "bullet/src/btBulletCollisionCommon.h"

namespace HellEngine
{
	class Window
	{

	public: // methods
		Window(float xPos, float zPos, int story, float height, Axis axis);
		void Draw(Shader* shader);
		void Reconfigure();

	public: // members
		Transform m_transform;
		float m_startHeight;
		Axis m_axis;
		int m_story;
		btCollisionObject* m_collisionObject; // for mouse picking in the level editor
	};
}