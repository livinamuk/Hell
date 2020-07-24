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
		Shell(Transform transform, glm::vec3 initialVelocity, CasingType casingType);
		void Update(float deltaTime);
		void Draw(Shader* shader);

		btRigidBody* m_rigidBody; 
		glm::mat4 m_modelMatrix;
		float m_shellScale = 1.6f;
		float m_timeSinceHitTheGround = 0;
		float m_lifeTime = 0;
		CasingType m_casingType;

		static std::vector<Shell> s_shotgunShells;
		static std::vector<Shell> s_bulletCasings;
	};
}