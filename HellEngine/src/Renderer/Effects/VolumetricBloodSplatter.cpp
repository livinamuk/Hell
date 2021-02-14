#include "hellpch.h"
#include "VolumetricBloodSplatter.h"
#include "Config.h"
#include "Helpers/Util.h"
#include "Core/Input.h"

namespace HellEngine
{
	GLuint VolumetricBloodSplatter::s_vao;
	GLuint VolumetricBloodSplatter::s_buffer_mode_matrices;
	std::vector<VolumetricBloodSplatter> VolumetricBloodSplatter::s_volumetricBloodSplatters;

	VolumetricBloodSplatter::VolumetricBloodSplatter(glm::vec3 position, glm::vec3 rotation, glm::vec3 front,  bool renderDecalOnly)
	{
		m_transform.position = position;
		m_transform.rotation = rotation;
		m_front = front;
		m_renderDecalOnly = renderDecalOnly;

		static int rand = 0;

		if (rand == 0)
			m_type = 7;
		else if (rand == 1)
			m_type = 9;
		else if (rand == 2)
			m_type = 6;
		else if (rand == 3)
			m_type = 4;
		//else if (rand == 4)
		//	m_type = 8;

		rand++;
		if (rand == 4)
			rand = 0;
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


	glm::mat4 VolumetricBloodSplatter::GetDecalModelMatrix()
	{
		Transform transform;

		transform.position = m_transform.position;
		transform.position.y = 0;

		transform.rotation = m_transform.rotation;
		transform.rotation.x = 0.4;// Config::TEST_FLOAT2;
		transform.rotation.z = 0;// Config::TEST_FLOAT3;

		transform.scale = glm::vec3(Config::TEST_FLOAT2, Config::TEST_FLOAT3, Config::TEST_FLOAT4);;
		transform.scale = glm::vec3(3, 5, 1.5f);;
		transform.scale = glm::vec3(3, 5, 1.5f) * glm::vec3(0.75);
		//transform.scale = glm::vec3(Config::TEST_FLOAT2, Config::TEST_FLOAT3, Config::TEST_FLOAT4) * glm::vec3(Config::TEST_FLOAT5);

		Transform decalOffset;

		if (m_type == 9)
			decalOffset.position = glm::vec3(0, 0, 0.475f);
		else if (m_type == 7)
			decalOffset.position = glm::vec3(0);
		else if (m_type == 6)
			decalOffset.position = glm::vec3(-0.0199999996, 0, 0.4699999988);
		else if (m_type == 4)
			decalOffset.position = glm::vec3(0, 0, 0.5f);
		else if (m_type == 8)
			decalOffset.position = glm::vec3(-0.0199999996f, 0.0000000000f, 0.3600000143f);

		return transform.to_mat4() * decalOffset.to_mat4();
	}


	/////////////////////////
	//                     //
	//   DRAW INSTANCIED   //
	//                     //
	/////////////////////////

	void VolumetricBloodSplatter::Init()
	{
		glGenBuffers(1, &s_buffer_mode_matrices);

		float vertices[] = {
			// positions
			0.5f,  0.5f,  0.5f,
			0.5f,  0.5f, -0.5f, 
			-0.5f,  0.5f, -0.5f,
			-0.5f,  0.5f, -0.5f, 
			-0.5f,  0.5f,  0.5f, 
			0.5f,  0.5f,  0.5f,

			0.5f,  0.5f, -0.5f,
			0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,  
			-0.5f,  0.5f, -0.5f,  
			0.5f, 0.5f, -0.5f,

			-0.5f, -0.5f,  0.5f,
			0.5f, -0.5f,  0.5f, 
			0.5f,  0.5f,  0.5f,
			0.5f,  0.5f,  0.5f, 
			-0.5f,  0.5f,  0.5f,
			-0.5f, -0.5f,  0.5f,

			-0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f, -0.5f, 
			-0.5f, -0.5f, -0.5f, 
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,

			0.5f, -0.5f, -0.5f,
			0.5f,  0.5f, -0.5f,
			0.5f,  0.5f,  0.5f,
			0.5f,  0.5f,  0.5f,
			0.5f, -0.5f,  0.5f,
			0.5f, -0.5f, -0.5f,

			-0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, -0.5f, 
			0.5f, -0.5f,  0.5f, 
			0.5f, -0.5f,  0.5f, 
			-0.5f, -0.5f,  0.5f, 
			-0.5f, -0.5f, -0.5f,
		};
		unsigned int VBO;
		glGenVertexArrays(1, &s_vao);
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glBindVertexArray(s_vao);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	}
	

	void VolumetricBloodSplatter::DrawInstancedDecals(Shader* shader)
	{
		// No decals? Then get outta here.
		if (!s_volumetricBloodSplatters.size())
			return;

		// No you can choose to only update this model matrix vector when a new decal is created. OR, a door is moving. 
		// This makes a lot of sense and has to be faster, but by how much, worth looking into, it's an easy win.

		static std::vector<glm::mat4> modelMatrixVector;

		if (Input::s_keyPressed[HELL_KEY_O])
		for (VolumetricBloodSplatter& splatter : s_volumetricBloodSplatters) {
			modelMatrixVector.push_back(splatter.GetDecalModelMatrix());
		}

		if (!modelMatrixVector.size())
			return;

		shader->setFloat("u_Time", 0);
		shader->setVec3("_DecalForwardDirection", s_volumetricBloodSplatters[0].m_front);
		shader->setInt("u_Type", s_volumetricBloodSplatters[0].m_type);

		glBindVertexArray(s_vao);

		glBindBuffer(GL_ARRAY_BUFFER, s_buffer_mode_matrices);
		glBufferData(GL_ARRAY_BUFFER, s_volumetricBloodSplatters.size() * sizeof(glm::mat4), &modelMatrixVector[0], GL_STATIC_DRAW);


		// vertex attributes
		std::size_t vec4Size = sizeof(glm::vec4);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));
		glVertexAttribDivisor(1, 1);
		glVertexAttribDivisor(2, 1);
		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);

		//draw
		glUniform1i(glGetUniformLocation(shader->ID, "instanced"), 1);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 36, s_volumetricBloodSplatters.size());
		glUniform1i(glGetUniformLocation(shader->ID, "instanced"), 0);
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

		Model* m_model = NULL;

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

		glm::mat4 modelMatrix = GetDecalModelMatrix();
		shader->setMat4("model", modelMatrix);
		shader->setMat4("inverseModel", glm::inverse(modelMatrix));

		shader->setInt("u_Type", this->m_type);

		glBindVertexArray(s_vao);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}