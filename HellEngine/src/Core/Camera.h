#pragma once
#include "Core/Input.h"
#include "Renderer/Transform.h"

namespace HellEngine
{
	class Camera
	{
	public:	// Methods
		Camera();
		void CalculateMatrices(glm::vec3 worldPos, glm::mat4 weaponCameraMatrix);
		void CalculateProjectionMatrix(int screenWidth, int screenHeight);
		void Update(float deltaTime);

	private: // Methods
		void MouseLook(float deltaTime);
		void PlayerMovement(float deltaTime);

	public: // Fields
		float m_oldX, m_oldY;		// Old mouse position
		float m_xoffset, m_yoffset;	// Distance mouse moved during current frame in pixels
		bool m_disable_MouseLook = false;

		Transform m_transform;
		//HeadbobController headbobController;

		glm::vec3 m_Front = glm::vec3(0, 1, 0);
		glm::vec3 m_Up;
		glm::vec3 m_Right;
		glm::vec3 m_WorldUp;

		glm::mat4 m_projectionViewMatrix;
		glm::mat4 m_projectionMatrix;
		glm::mat4 m_viewMatrix;
		glm::mat4 m_inverseViewMatrix;
		glm::mat4 m_inversePprojectionMatrix;
	
		float m_mmouseSensitivity;
		
		glm::vec3 m_viewPos = glm::vec3(0);
		float m_viewHeight = 1.0f;

	};
}