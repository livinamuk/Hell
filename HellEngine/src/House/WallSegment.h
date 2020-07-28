#pragma once
#include "Header.h"
#include "House/WallMesh.h"
#include "House/Door.h"
#include "House/Floor.h"
#include "House/Ceiling.h"
#include "House/Window.h"
#include "House/Staircase.h"
#include "bullet/src/btBulletCollisionCommon.h"

namespace HellEngine
{
	class WallSegment
	{
	public: // methods
		WallSegment(glm::vec3 position, Axis axis, float width, float heightOfUpperSegment, float heightOfLowerSegment);
		~WallSegment();
		void RemoveCollisionObjects();

	public: // members
		glm::vec3 m_position;
		Axis m_axis;
		float m_width = 0;
		float m_heightOfUpperSegment = 0;
		float m_heightOfLowerSegment = 0;
		btCollisionObject* m_collisionObject_UpperSegment = nullptr;
		btCollisionObject* m_collisionObject_LowerSegment = nullptr;
	};
}