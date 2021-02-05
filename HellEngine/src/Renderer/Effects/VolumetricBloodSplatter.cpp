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
	bloodMeshTransform.rotation = glm::vec3(0);
	bloodMeshTransform.scale = glm::vec3(0.5f);
	bloodMeshTransform.position = m_transform.position;
	bloodMeshTransform.rotation = m_transform.rotation;

	Transform bloodMeshOffset;
	bloodMeshOffset.position = glm::vec3(-0.45f, -1.96f, -1);
	bloodMeshOffset.rotation = glm::vec3(0);
	bloodMeshOffset.scale = glm::vec3(0.0003f);

	Transform rotTransform; // Rotates mesh 90 degress around Y axis
	rotTransform.rotation = glm::vec3( 0, -HELL_PI / 2, 0);

	return bloodMeshTransform.to_mat4() * rotTransform.to_mat4() * bloodMeshOffset.to_mat4();
}

	void VolumetricBloodSplatter::Draw(Shader* shader)
	{
		shader->use();
		shader->setMat4("u_MatrixWorld", this->GetModelMatrix());
		shader->setFloat("u_Time", this->m_CurrentTime);

		Model* m_model = AssetManager::GetModelByName("blood_mesh");

		int VAO = m_model->m_meshes[0]->VAO;
		int numIndices = m_model->m_meshes[0]->indices.size();
		int numVerts = m_model->m_meshes[0]->vertices.size();

		glBindVertexArray(VAO);
		//glPointSize(4);
		glDrawElements(GL_TRIANGLES, (GLsizei)numIndices, GL_UNSIGNED_INT, 0);
	}
}