#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Renderer/Transform.h"
#include "header.h"
#include <memory>
#include "Core/Camera.h"
#include "Renderer/Texture.h"

class Quad {
public:
	Quad() {

		float vertices[] = {
			-0.5f, +0.5f, +0.0f, 0.0f, 0.0f,
			+0.5f, +0.5f, +0.0f, 1.0f, 0.0f,
			+0.5f, -0.5f, +0.0f, 1.0f, 1.0f,
			-0.5f, -0.5f, +0.0f, 0.0f, 1.0f
		};
		uint32_t indices[] = { 0, 1, 2, 0, 2, 3 };

		glGenVertexArrays(1, &m_VAO);
		glBindVertexArray(m_VAO);

		glGenBuffers(1, &m_VBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(float)));

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glGenBuffers(1, &m_IBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glBindVertexArray(0);
	}


	auto Draw() -> void {
		glBindVertexArray(m_VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

private:
	GLuint m_VAO;
	GLuint m_VBO;
	GLuint m_IBO;
};

class BloodEffect
{
public: // fields
	//struct CreateDesc {

		Transform TransformHead;
		Transform TransformRight;
		Transform TransformMain;
		Transform TransformLeft;
		std::shared_ptr<Texture> TextureHead;
		std::shared_ptr<Texture> TextureRight;
		std::shared_ptr<Texture> TextureMain;
		std::shared_ptr<Texture> TextureLeft;
		uint32_t                 CountRaw;
		uint32_t                 CountColumn;
		float                    AnimationSeconds;
	//};private:
	//	CreateDesc m_Desc;

		std::unique_ptr<Quad>   m_pQuad;
		//std::unique_ptr<Shader> m_pShaderObject;

		int32_t                 m_FrameIndex = 0;
		float                   m_Interpolate = 0.0f;
		float                   m_CurrentTime = 0.0f;
	

public: // methods
	BloodEffect();
	void Draw(Shader* shader, HellEngine::Camera const& camera, Transform& global);
	void Update(float deltaTime);
	void Init();
};