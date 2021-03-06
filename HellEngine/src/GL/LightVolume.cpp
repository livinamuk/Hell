#include "hellpch.h"
#include "LightVolume.h"
#include "GpuProfiling.h"

namespace HellEngine
{
	LightVolume::LightVolume()
	{
		this->VAO = 0;
	}

	void LightVolume::Rebuild()
	{
		
	}

	void LightVolume::BuildFromRoom(Room* room)
	{
		float bias = 0.02f;
		vertices.clear();
		float lowerX = room->m_lowerX - bias;
		float lowerZ = room->m_lowerZ - bias;
		float upperX = room->m_upperX + bias;
		float upperZ = room->m_upperZ + bias;

		// Create vertices
		glm::vec3 A1 = glm::vec3(lowerX, -bias, lowerZ);
		glm::vec3 B1 = glm::vec3(upperX, -bias, lowerZ);
		glm::vec3 C1 = glm::vec3(lowerX, -bias, upperZ);
		glm::vec3 D1 = glm::vec3(upperX, -bias, upperZ);
		glm::vec3 A2 = glm::vec3(lowerX, 2.4f + bias, lowerZ);
		glm::vec3 B2 = glm::vec3(upperX, 2.4f + bias, lowerZ);
		glm::vec3 C2 = glm::vec3(lowerX, 2.4f + bias, upperZ);
		glm::vec3 D2 = glm::vec3(upperX, 2.4f + bias, upperZ);
		AddCuboidToLightVolume(A1, B1, C1, D1, A2, B2, C2, D2);

		Setup();
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

	void LightVolume::AddCubeToLightVolume(float lowerX, float upperX, float lowerY, float upperY, float lowerZ, float upperZ, float bias)
	{
		glm::vec3 A1 = glm::vec3(lowerX - bias, lowerY - bias, lowerZ - bias);
		glm::vec3 B1 = glm::vec3(upperX + bias, lowerY - bias, lowerZ - bias);
		glm::vec3 C1 = glm::vec3(lowerX - bias, lowerY - bias, upperZ + bias);
		glm::vec3 D1 = glm::vec3(upperX + bias, lowerY - bias, upperZ + bias);
		glm::vec3 A2 = glm::vec3(lowerX - bias, upperY + bias, lowerZ - bias);
		glm::vec3 B2 = glm::vec3(upperX + bias, upperY + bias, lowerZ - bias);
		glm::vec3 C2 = glm::vec3(lowerX - bias, upperY + bias, upperZ + bias);
		glm::vec3 D2 = glm::vec3(upperX + bias, upperY + bias, upperZ + bias);
		this->AddCuboidToLightVolume(A1, B1, C1, D1, A2, B2, C2, D2);
	}


	void LightVolume::Draw(Shader* shader)
	{
		GpuProfiler g("LightVolume");
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