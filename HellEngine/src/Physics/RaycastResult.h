#pragma once

#include "Physics.h"
#include "Core/Camera.h"
#include "Helpers/Util.h"

namespace HellEngine
{
	class RaycastResult 
	{
	public: // methods
		RaycastResult();
		void CastRay(glm::vec3 rayOrigin, glm::vec3 rayDirection, float rayLength, float variance = 0);

	public: // members
		int m_elementIndex; 
		float m_distance;
		glm::vec3 m_hitPoint;
		glm::vec3 m_surfaceNormal;
		btRigidBody* m_rigidBody = nullptr;
		char* m_name;
	};
}