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

		int m_type = 9;
		bool m_renderDecalOnly;

	public: // methods
		VolumetricBloodSplatter(glm::vec3 position, glm::vec3 rotation, glm::vec3 front, bool renderDecalOnly = false);
		void Update(float deltaTime);
		void Draw(Shader* shader); 
		void DrawDecal(Shader* shader);
		glm::mat4 GetModelMatrix();
		glm::mat4 GetDecalModelMatrix();

	public: // static functions
		static void Init();
		static void DrawInstancedDecals(Shader* shader);

	public: // static Variables
		static GLuint s_buffer_mode_matrices;
		static GLuint s_vao;
		static std::vector<VolumetricBloodSplatter> s_volumetricBloodSplatters;
	};
}