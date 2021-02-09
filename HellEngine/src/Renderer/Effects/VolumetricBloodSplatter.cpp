#include "hellpch.h"
#include "VolumetricBloodSplatter.h"
#include "Config.h"
#include "Helpers/Util.h"

namespace HellEngine
{
	VolumetricBloodSplatter::VolumetricBloodSplatter(glm::vec3 position, glm::vec3 rotation, glm::vec3 front,  bool renderDecalOnly)
	{
		m_transform.position = position;
		m_transform.rotation = rotation;
		m_front = front;
		m_renderDecalOnly = renderDecalOnly;

		static int rand = 0;

		//int rand = Util::GetRandomInt(0, 4);

		if (rand == 0)
			m_type = 7;
		else if (rand == 1)
			m_type = 9;
		else if (rand == 2)
			m_type = 6;
		else if (rand == 3)
			m_type = 4;
		else if (rand == 4)
			m_type = 8;

		rand++;
		if (rand == 4)
			rand = 0;

		//m_type = 8;
		
		// its like 9 iswrong as well


	//	m_type = 1;
	//	m_type = 7;

		std::cout << "RAND: " << rand << " " << m_type << "\n";
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
		else if (m_type == 7)
			bloodMeshOffset.position = glm::vec3(-0.2300000042f, -0.5000000000f, -0.2249999940f);
		else if (m_type == 6)
			bloodMeshOffset.position = glm::vec3(-0.0839999989, -0.3799999952, -0.1500000060);
		else if (m_type == 8)
			bloodMeshOffset.position = glm::vec3(-0.1700000018f, -0.2290000021f, -0.1770000011f);
		else if (m_type == 9)
			bloodMeshOffset.position = glm::vec3(-0.0500000007, -0.2549999952, -0.1299999952);
		else if (m_type == 4)
			bloodMeshOffset.position = glm::vec3(-0.0500000045f, -0.4149999917f, -0.1900000125f);
		
		//	bloodMeshOffset.position = glm::vec3(Config::TEST_FLOAT, Config::TEST_FLOAT2, Config::TEST_FLOAT3);

		//bloodMeshOffset.position = glm::vec3(Config::TEST_FLOAT, Config::TEST_FLOAT2, Config::TEST_FLOAT3);
			//bloodMeshOffset.position = glm::vec3(-0.0500000007, -0.2549999952, -0.1299999952);
		//	bloodMeshOffset.position = glm::vec3(-0.231f, -0.492f, -0.225f);

		//bloodMeshOffset.position = glm::vec3(Config::TEST_FLOAT, Config::TEST_FLOAT2, Config::TEST_FLOAT3);

		Transform rotTransform; // Rotates mesh 90 de	gress around Y axis
		rotTransform.rotation = glm::vec3(0, -HELL_PI / 2, 0);
		//rotTransform.rotation = glm::vec3(Config::TEST_FLOAT, Config::TEST_FLOAT2, Config::TEST_FLOAT3);

		Transform scaleTransform;
		scaleTransform.scale = glm::vec3(3);

		return bloodMeshTransform.to_mat4() * scaleTransform.to_mat4() * rotTransform.to_mat4() * bloodMeshOffset.to_mat4();
	}

	void VolumetricBloodSplatter::Draw(Shader* shader)
	{
		if (m_renderDecalOnly)
			return;

		// Bail if time too long
		if (m_CurrentTime > 0.9)
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
		else if (m_type == 7) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, HellEngine::AssetManager::s_ExrTexture_pos7.gTexId);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, HellEngine::AssetManager::s_ExrTexture_norm7.gTexId);
			m_model = AssetManager::GetModelByName("blood_mesh7");
		}
		else if (m_type == 6) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, HellEngine::AssetManager::s_ExrTexture_pos6.gTexId);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, HellEngine::AssetManager::s_ExrTexture_norm6.gTexId);
			m_model = AssetManager::GetModelByName("blood_mesh6");
		}
		else if (m_type == 8) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, HellEngine::AssetManager::s_ExrTexture_pos8.gTexId);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, HellEngine::AssetManager::s_ExrTexture_norm8.gTexId);
			m_model = AssetManager::GetModelByName("blood_mesh8");
		}
		else if (m_type == 9) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, HellEngine::AssetManager::s_ExrTexture_pos9.gTexId);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, HellEngine::AssetManager::s_ExrTexture_norm9.gTexId);
			m_model = AssetManager::GetModelByName("blood_mesh9");
		}
		else if (m_type == 4) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, HellEngine::AssetManager::s_ExrTexture_pos4.gTexId);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, HellEngine::AssetManager::s_ExrTexture_norm4.gTexId);
			m_model = AssetManager::GetModelByName("blood_mesh4");
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
		transform.position.y = 0;

		transform.rotation = m_transform.rotation;
		transform.rotation.x = 0.4;// Config::TEST_FLOAT2;
		transform.rotation.z = 0;// Config::TEST_FLOAT3;

		//transform.rotation.y = m_transform.rotation.y;
	//	transform.rotation.z = 0.3f;
	//	transform.rotation.y = m_transform.rotation.y;
		transform.scale = glm::vec3(Config::TEST_FLOAT2, Config::TEST_FLOAT3, Config::TEST_FLOAT4);;
		transform.scale = glm::vec3(3, 5, 1.5f);;
		transform.scale = glm::vec3(3, 5, 1.5f) * glm::vec3(0.75);

		
		//glm::vec3 squareNormal = glm::vec3(0, 0, 1);

		Transform decalOffset;


		if (m_type == 9) {
			decalOffset.position = glm::vec3(0, 0, 0.475f);
		}
		else if (m_type == 7) {
			decalOffset.position = glm::vec3(0);
		}
		else if (m_type == 6) {
			decalOffset.position = glm::vec3(-0.0199999996, 0, 0.4699999988);
		}
		else if (m_type == 4) {
			decalOffset.position = glm::vec3(0, 0, 0.5f);
		}
		else if (m_type == 8) {
			decalOffset.position = glm::vec3(-0.0199999996f, 0.0000000000f, 0.3600000143f);
		}

		//	decalOffset.position = glm::vec3(Config::TEST_FLOAT2, Config::TEST_FLOAT3, Config::TEST_FLOAT4);
		//decalOffset.position = glm::vec3(Config::TEST_FLOAT2, Config::TEST_FLOAT3, Config::TEST_FLOAT4); //decalOffset.position = glm::vec3(-1.3200000525, 0, 0);

		glm::mat4 modelMatrix = transform.to_mat4() * decalOffset.to_mat4();
		shader->setMat4("model", modelMatrix);
		shader->setMat4("inverseModel", glm::inverse(modelMatrix));



		if (m_type == 7) {
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, AssetManager::GetTexIDByName("decal_norm7"));
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, AssetManager::GetTexIDByName("decal_mask7"));
		}
		else if (m_type == 6) {
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, AssetManager::GetTexIDByName("decal_norm6"));
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, AssetManager::GetTexIDByName("decal_mask6"));
		}
		else if (m_type == 9) {
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, AssetManager::GetTexIDByName("decal_norm9"));
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, AssetManager::GetTexIDByName("decal_mask9"));
		}
		else if (m_type == 4) {
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, AssetManager::GetTexIDByName("decal_norm4"));
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, AssetManager::GetTexIDByName("decal_mask4"));
		}
		else if (m_type == 8) {
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, AssetManager::GetTexIDByName("decal_norm8"));
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, AssetManager::GetTexIDByName("decal_mask8"));
		}

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