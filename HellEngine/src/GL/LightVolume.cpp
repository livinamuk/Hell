#include "hellpch.h"
#include "LightVolume.h"

namespace HellEngine
{
	LightVolume::LightVolume()
	{
		this->VAO = 0;
	}

	void LightVolume::AddCuboidToLightVolume(glm::vec3 A1, glm::vec3 B1, glm::vec3 C1, glm::vec3 D1, glm::vec3 A2, glm::vec3 B2, glm::vec3 C2, glm::vec3 D2)
	{
		// Floor
		vertices.push_back(A1);
		vertices.push_back(B1);
		vertices.push_back(C1);
		vertices.push_back(D1);
		vertices.push_back(C1);
		vertices.push_back(B1);
		// Ceiling
		vertices.push_back(C2);
		vertices.push_back(B2);
		vertices.push_back(A2);
		vertices.push_back(B2);
		vertices.push_back(C2);
		vertices.push_back(D2);
		// Side wall
		vertices.push_back(C1);
		vertices.push_back(A2);
		vertices.push_back(A1);
		vertices.push_back(C2);
		vertices.push_back(A2);
		vertices.push_back(C1);
		// Side wall the other one
		vertices.push_back(B1);
		vertices.push_back(B2);
		vertices.push_back(D1);
		vertices.push_back(D1);
		vertices.push_back(B2);
		vertices.push_back(D2);
		// Front wall
		vertices.push_back(A1);
		vertices.push_back(B2);
		vertices.push_back(B1);
		vertices.push_back(A1);
		vertices.push_back(A2);
		vertices.push_back(B2);
		// Back wall
		vertices.push_back(D1);
		vertices.push_back(D2);
		vertices.push_back(C1);
		vertices.push_back(D2);
		vertices.push_back(C2);
		vertices.push_back(C1);
	}


	void LightVolume::Draw(Shader* shader)
	{
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		shader->setMat4("model", modelMatrix);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
		//glDrawArrays(GL_POINTS, 0, vertices.size());
	}

	void LightVolume::Setup()
	{
		unsigned int VBO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
		glBindVertexArray(VAO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	}
}