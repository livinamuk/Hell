#include "hellpch.h"
#include "Decal.h"
#include "Renderer/Cube.h"
#include "Helpers/Util.h"
#include "Helpers/AssetManager.h"

namespace HellEngine
{
	std::vector<Decal> Decal::s_decals;

	Decal::Decal(glm::vec3 position, glm::vec3 normal)
	{
		//this->decalType = decalType;
		this->transform.position = position;
		this->normal = normal;// *glm::vec3(-1);
		this->randomRotation = Util::RandomFloat(0, HELL_PI * 2);

		//if (decalType == DecalType::BULLET_HOLE)
			transform.scale = glm::vec3(0.025f);
	}

	void Decal::Draw(Shader* shader, bool blackOnly)
	{
		// Calculate rotation
		glm::mat4 modelMatrix = glm::translate(glm::mat4(1), transform.position);
		glm::vec3 squareNormal = glm::vec3(0, 0, 1);
		float angle = glm::acos(glm::dot(normal, squareNormal));
		if (angle > 0.001f)
		{
			if (angle == HELL_PI)
				modelMatrix = glm::rotate(modelMatrix, angle, glm::vec3(0, 1, 0));
			else {
				glm::vec3 axis = glm::normalize(glm::cross(squareNormal, normal));
				modelMatrix = glm::rotate(modelMatrix, angle, axis);
		 	}
		}
		// Draw the cunt
		modelMatrix = glm::rotate(modelMatrix, randomRotation, glm::vec3(0, 0, 1));
		modelMatrix = glm::scale(modelMatrix, transform.scale);
		shader->setMat4("model", modelMatrix);

		glActiveTexture(GL_TEXTURE2);
//		if (!blackOnly)
//			glBindTexture(GL_TEXTURE_2D, AssetManager::GetTexIDByName("BulletHole1_BaseColor"));
//		else
//			glBindTexture(GL_TEXTURE_2D, AssetManager::GetTexIDByName("BulletHole1Black_BaseColor"));


		glBindTexture(GL_TEXTURE_2D, AssetManager::GetTexIDByName("Bullet"));
	//	if (!blackOnly)
	//		AssetManager::BindMaterial(AssetManager::GetMaterialIDByName("BulletHole1"));
	//	else
	//		AssetManager::BindMaterial(AssetManager::GetMaterialIDByName("BulletHole1Black"));


		//shader->setMat4("model", glm::mat4(1));

		static unsigned int VAO = 0;
		if (VAO == 0) {
			float vertices[] = {
				// positions          // normals           // texture coords	
				0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,	// top
				0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
				-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
				-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
				-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
				0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,

				0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,	// front
				0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
				-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
				-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
				-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
				0.5f, 0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

				-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f, // back
				0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
				0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
				0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
				-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
				-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

				-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f, // right
				-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
				-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
				-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
				-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
				-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

				0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f, // left
				0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
				0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
				0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
				0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
				0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

				-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f, // bottom
				0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
				0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
				0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
				-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
				-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f
			};
			unsigned int VBO;
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
			glBindVertexArray(VAO);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
			glEnableVertexAttribArray(2);
		}

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

	//	std::cout << "DREW SOMETHING\n";

		//glBindVertexArray(Cube::VAO);
		///glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}

