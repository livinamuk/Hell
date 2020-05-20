#pragma once
#include "hellpch.h"
#include "Camera.h"
#include "Header.h"
#include "Helpers/Util.h"

namespace HellEngine
{
	Camera::Camera()
	{
		m_transform.position = glm::vec3(0, 0.0f, 2);
		m_transform.rotation = glm::vec3(0.125f, 0, 0);
		m_transform.scale = glm::vec3(1);
		m_oldX = 1280 / 2;
		m_oldY = 720 / 2;
		Input::s_mouseX = m_oldX;
		Input::s_mouseY = m_oldY;
	}

	void Camera::CalculateMatrices(glm::vec3 worldPos, glm::mat4 shotgunMatrix)
	{

		m_transform.position = worldPos;
		m_transform.position.y += m_viewHeight;
		m_viewMatrix = shotgunMatrix * glm::inverse(m_transform.to_mat4());

		m_inverseViewMatrix = glm::inverse(m_viewMatrix);

		glm::mat4 worldMatrix = m_transform.to_mat4();
		m_Right = glm::vec3(worldMatrix[0]);
		m_Up = glm::vec3(worldMatrix[1]);
		m_Front = glm::vec3(worldMatrix[2]);

		m_viewPos.x = m_inverseViewMatrix[3][0];
		m_viewPos.y = m_inverseViewMatrix[3][1];
		m_viewPos.z = m_inverseViewMatrix[3][2];


		glm::vec4 vP = (m_inverseViewMatrix * glm::vec4(0, 0, 0, 1));
		m_viewPos = glm::vec3(vP.x, vP.y, vP.z);

		/*	Transform cameraTransform = this->m_transform;
			//cameraTransform.position.y += m_viewHeight;
			glm::mat4 cameraMatrix = cameraTransform.to_mat4();

			m_viewMatrix = weaponCameraMatrix * glm::inverse(cameraMatrix);
			m_projectionViewMatrix = m_projectionMatrix * m_viewMatrix;
			m_inverseViewMatrix = glm::inverse(m_viewMatrix);

			m_Right = glm::vec3(cameraMatrix[0]);
			m_Up = glm::vec3(cameraMatrix[1]);
			m_Front = glm::vec3(cameraMatrix[2]);

			m_viewPos.x = m_inverseViewMatrix[3][0];
			m_viewPos.y = m_inverseViewMatrix[3][1];
			m_viewPos.z = m_inverseViewMatrix[3][2];*/
	}

	void Camera::CalculateProjectionMatrix(int screenWidth, int screenHeight)
	{
		m_projectionMatrix = glm::perspective(1.00f, (float)screenWidth / (float)screenHeight, NEAR_PLANE, FAR_PLANE);
	}

	void Camera::Update(float deltaTime)
	{
		MouseLook(deltaTime);
		PlayerMovement(deltaTime);
	}

	void Camera::MouseLook(float deltaTime)
	{
		m_xoffset = (float)Input::s_mouseX - m_oldX;
		m_yoffset = m_oldY - (float)Input::s_mouseY;
		m_oldX = (float)Input::s_mouseX;
		m_oldY = (float)Input::s_mouseY;

		if (Input::s_showCursor)
			return;

		float yLimit = 1.5f;
		m_transform.rotation += glm::vec3(-m_yoffset, -m_xoffset, 0.0) / glm::vec3(200);
		m_transform.rotation.x = std::min(m_transform.rotation.x, yLimit);
		m_transform.rotation.x = std::max(m_transform.rotation.x, -yLimit);
	}

	void Camera::PlayerMovement(float deltaTime)
	{
		float cameraSpeed = 2.5 * deltaTime;
		glm::vec3 Front = m_Front;
		Front.y = 0;
		Front = glm::normalize(Front);
	}
}