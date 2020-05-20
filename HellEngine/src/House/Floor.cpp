#include "hellpch.h"
#include "Floor.h"
#include "Helpers/Util.h"
#include "Helpers/AssetManager.h"

namespace HellEngine
{
	Floor::Floor()
	{
	}

	Floor::Floor(glm::vec3 position, glm::vec2 size, int story, bool rotateTexture)
	{
		m_transform.position.x = position.x;
		m_transform.position.y = STORY_HEIGHT * story;
		m_transform.position.z = position.z;
		m_transform.scale.x = size.x;
		m_transform.scale.z = size.y;
		m_rotateTexture = rotateTexture;
		CalculateWorldSpaceCorners();
	}

	Floor::Floor(glm::vec3 position, glm::vec2 size) // for stairs
	{
		m_transform.position = position;
		m_transform.scale.x = size.x;
		m_transform.scale.z = size.y;
		CalculateWorldSpaceCorners();
	}

	void Floor::Draw(Shader* shader)
	{
		if (m_rotateTexture)
			shader->setInt("TEXTURE_FLAG", 3);
		else
			shader->setInt("TEXTURE_FLAG", 4);

		Util::DrawUpFacingPlane(shader, &m_transform);
		shader->setInt("TEXTURE_FLAG", 0);
	}

	void Floor::CalculateWorldSpaceCorners()
	{
		glm::vec3 a = m_transform.to_mat4() * glm::vec4(glm::vec3(-0.5f, 0, 0.5f), 1.0f);
		glm::vec3 b = m_transform.to_mat4() * glm::vec4(glm::vec3(0.5f, 0, 0.5f), 1.0f);
		glm::vec3 c = m_transform.to_mat4() * glm::vec4(glm::vec3(0.5f, 0, -0.5f), 1.0f);
		glm::vec3 d = m_transform.to_mat4() * glm::vec4(glm::vec3(-0.5f, 0, -0.5f), 1.0f);

		worldSpaceCorners.clear();
		worldSpaceCorners.push_back(glm::vec3(a));
		worldSpaceCorners.push_back(glm::vec3(b));
		worldSpaceCorners.push_back(glm::vec3(c));
		worldSpaceCorners.push_back(glm::vec3(d));
	}
}