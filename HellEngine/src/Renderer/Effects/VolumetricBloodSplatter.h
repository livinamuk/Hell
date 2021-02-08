#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Renderer/Transform.h"
#include "header.h"
#include "Renderer/Model.h"
#include "Helpers/AssetManager.h"

namespace HellEngine
{
	class VolumetricBloodSplatter
	{
	public: // fields
		float m_CurrentTime = 0.0f;
		Transform m_transform;
		Model* m_model;
		glm::vec3 m_front;

		int m_type = 1;

	public: // methods
		VolumetricBloodSplatter(glm::vec3 position, glm::vec3 rotation, glm::vec3 front);
		void Update(float deltaTime);
		void Draw(Shader* shader); 
		void DrawDecal(Shader* shader);
		glm::mat4 GetModelMatrix();
	};
}