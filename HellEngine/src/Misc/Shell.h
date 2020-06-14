#pragma once

#include "header.h"
#include "Physics/Physics.h"

namespace HellEngine
{
	class Shell
	{
	public: // fields
		Transform m_transform;

	public: // methods
		Shell(Transform transform, glm::vec3 initialVelocity);
		void Update(float deltaTime);
		void Draw(Shader* shader);

		btRigidBody* m_rigidBody;

		float shellScale = 1.6f;

		static std::vector<Shell> s_shells;
	};
}