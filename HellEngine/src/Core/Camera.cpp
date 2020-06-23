#pragma once
#include "hellpch.h"
#include "Camera.h"
#include "Header.h"
#include "Helpers/Util.h"
#include "Core/CoreGL.h"
#include "Config.h"

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

	void Camera::CalculateMatrices(glm::vec3 viewPosition)
	{

		m_transform.position = viewPosition;
		//m_transform.position.y += m_viewHeight;
		m_viewMatrix = inverse(m_weaponCameraMatrix) * glm::inverse(m_transform.to_mat4());

		m_inverseViewMatrix = glm::inverse(m_viewMatrix);

		glm::mat4 worldMatrix = (m_weaponCameraMatrix) * m_transform.to_mat4();
		m_Right = glm::vec3(worldMatrix[0]);
		m_Up = glm::vec3(worldMatrix[1]);
		m_Front = glm::vec3(worldMatrix[2]);

		m_Right = glm::vec3(worldMatrix[0]); // glm::vec3(-1, -1, -1);
		m_Up = glm::vec3(worldMatrix[1]);// *glm::vec3(-1, -1, -1);
		m_Front = glm::vec3(worldMatrix[2]) * glm::vec3(-1, -1, -1);

		m_viewPos.x = m_inverseViewMatrix[3][0];
		m_viewPos.y = m_inverseViewMatrix[3][1];
		m_viewPos.z = m_inverseViewMatrix[3][2];


		glm::vec4 vP = (m_inverseViewMatrix * glm::vec4(0, 0, 0, 1));
		m_viewPos = glm::vec3(vP.x, vP.y, vP.z);

		m_projectionViewMatrix = m_projectionMatrix * m_viewMatrix;
	}

	void Camera::CalculateProjectionMatrix(int screenWidth, int screenHeight)
	{
		m_projectionMatrix = glm::perspective(1 - m_zoomFactor, (float)screenWidth / (float)screenHeight, NEAR_PLANE, FAR_PLANE);
	}

	void Camera::CalculateWeaponSwayTransform(float deltatime)
	{
		float movementX = - m_xoffset * Config::SWAY_AMOUNT;
		float movementY = -m_yoffset * Config::SWAY_AMOUNT;

		movementX = std::min(movementX, Config::SWAY_MAX_X);
		movementX = std::max(movementX, Config::SWAY_MIN_X);
		movementY = std::min(movementY, Config::SWAY_MAX_Y);
		movementY = std::max(movementY, Config::SWAY_MIN_Y);

		glm::vec3 finalPosition = glm::vec3(movementX, movementY, 0);

		m_weaponSwayTransform.position = Util::Vec3InterpTo(m_weaponSwayTransform.position, finalPosition, deltatime, Config::SMOOTH_AMOUNT);
	}

	void Camera::Update(float deltaTime)
	{
		MouseLook(deltaTime);
		PlayerMovement(deltaTime);
	}

	void Camera::MouseLook(float deltaTime)
	{
		m_xoffset = (float)Input::s_mouseX - m_oldX;
		m_yoffset = (float)Input::s_mouseY - m_oldY;
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