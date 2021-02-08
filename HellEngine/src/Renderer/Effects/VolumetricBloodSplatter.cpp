#include "hellpch.h"
#include "VolumetricBloodSplatter.h"
#include "Config.h"

namespace HellEngine
{
	VolumetricBloodSplatter::VolumetricBloodSplatter(glm::vec3 position, glm::vec3 rotation, glm::vec3 front)
	{
		m_transform.position = position;
		m_transform.rotation = rotation;
		m_front = front;
	}
	
	void VolumetricBloodSplatter::Update(float deltaTime)
	{
		m_CurrentTime += deltaTime;
	}

glm::mat4 VolumetricBloodSplatter::GetModelMatrix()
{
	Transform bloodMeshTransform;
	bloodMeshTransform.position = m_transform.position;
	bloodMeshTransform.rotation = m_transform.rotation;
	bloodMeshTransform.scale = glm::vec3(1.0f);


	Transform bloodMeshOffset;

	if (m_type == 0)
		bloodMeshOffset.position = glm::vec3(-0.08f, -0.23f, -0.155f);
	else if (m_type == 1)
			bloodMeshOffset.position = glm::vec3(-0.21f, -0.492f, -0.225f);
	//	bloodMeshOffset.position = glm::vec3(-0.231f, -0.492f, -0.225f);

	//bloodMeshOffset.position = glm::vec3(Config::TEST_FLOAT, Config::TEST_FLOAT2, Config::TEST_FLOAT3);

	Transform rotTransform; // Rotates mesh 90 degress around Y axis
	rotTransform.rotation = glm::vec3( 0, -HELL_PI / 2, 0);

	Transform scaleTransform;
	scaleTransform.scale = glm::vec3(3);

	return bloodMeshTransform.to_mat4() * scaleTransform.to_mat4() * rotTransform.to_mat4() * bloodMeshOffset.to_mat4();
}

	void VolumetricBloodSplatter::Draw(Shader* shader)
	{
		// Bail if time too long
		if (m_type == 1 && m_CurrentTime > 0.9)
			return;


		shader->use();
		shader->setMat4("u_MatrixWorld", this->GetModelMatrix());
		shader->setFloat("u_Time", this->m_CurrentTime);

		Model* m_model;


		if (m_type == 0) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, HellEngine::AssetManager::s_ExrTexture_pos.gTexId);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, HellEngine::AssetManager::s_ExrTexture_norm.gTexId);
			m_model = AssetManager::GetModelByName("blood_mesh");
		}
		else if (m_type == 1) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, HellEngine::AssetManager::s_ExrTexture_pos7.gTexId);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, HellEngine::AssetManager::s_ExrTexture_norm7.gTexId);
			m_model = AssetManager::GetModelByName("blood_mesh7");
		}


		int VAO = m_model->m_meshes[0]->VAO;
		int numIndices = m_model->m_meshes[0]->indices.size();
		int numVerts = m_model->m_meshes[0]->vertices.size();

		glBindVertexArray(VAO);
		//glPointSize(4);
		glDrawElements(GL_TRIANGLES, (GLsizei)numIndices, GL_UNSIGNED_INT, 0);
	}

	void VolumetricBloodSplatter::DrawDecal(Shader* shader)
	{
		shader->setFloat("u_Time", this->m_CurrentTime);
		shader->setVec3("_DecalForwardDirection", this->m_front);

		Transform transform;

		transform.position = m_transform.position;// glm::vec3(0);// s_DebugTransform.position;// debugtransfglm::vec3(-1, 1, 0.09f);
		transform.position.y = Config::TEST_FLOAT;
												  
		//	transform.position = glm::vec3(Config::TEST_FLOAT, Config::TEST_FLOAT2, Config::TEST_FLOAT3);

	//	transform.position = Config::BLOOD_MESH_POSITION;// s_DebugTransform.position;
	//	transform.position = glm::vec3(Config::TEST_FLOAT, Config::TEST_FLOAT2, Config::TEST_FLOAT3);
	//	transform.rotation = glm::vec3(0, 0, 0.3f);// s_DebugTransform.rotation;
		//transform.scale = glm::vec3(6, 10.1, 2.8) * glm::vec3(Config::TEST_FLOAT + 1);
		//	transform.scale = s_DebugTransform.scale;// glm::vec3(6, 0.1, 2.8);
		//	transform.scale = glm::vec3(11);

		transform.rotation = m_transform.rotation;
		transform.rotation.x = 0.4;// Config::TEST_FLOAT2;
		transform.rotation.z = 0;// Config::TEST_FLOAT3;

		//transform.rotation.y = m_transform.rotation.y;
	//	transform.rotation.z = 0.3f;
	//	transform.rotation.y = m_transform.rotation.y;
		transform.scale = glm::vec3(Config::TEST_FLOAT2, Config::TEST_FLOAT3, Config::TEST_FLOAT4);;
		transform.scale = glm::vec3(3, 5, 1.5f);;
		transform.scale = glm::vec3(3, 5, 1.5f) * glm::vec3(0.75);

		glm::mat4 modelMatrix = transform.to_mat4();// glm::translate(glm::mat4(1), transform.position);
		//glm::vec3 squareNormal = glm::vec3(0, 0, 1);

		shader->setMat4("model", modelMatrix);



	//	glActiveTexture(GL_TEXTURE2);
	//	glBindTexture(GL_TEXTURE_2D, AssetManager::GetTexIDByName("Decal_norm"));
	///	glActiveTexture(GL_TEXTURE3);
	//	glBindTexture(GL_TEXTURE_2D, AssetManager::GetTexIDByName("Decal_mask"));

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, AssetManager::GetTexIDByName("decal_norm7"));
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, AssetManager::GetTexIDByName("decal_mask7"));


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
	}
}