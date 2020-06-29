#include "hellpch.h"
#include "Ceiling.h"
#include "Helpers/Util.h"
#include "Helpers/AssetManager.h"
#include "GL/GpuProfiling.h"

namespace HellEngine
{
	Ceiling::Ceiling()
	{
	}

	Ceiling::Ceiling(glm::vec3 position, glm::vec2 size, int story, bool rotateTexture)
	{
		m_transform.position.x = position.x;
		m_transform.position.y = ROOM_HEIGHT + (STORY_HEIGHT * story);
		m_transform.position.z = position.z;
		m_transform.scale.x = size.x;
		m_transform.scale.y = 1;
		m_transform.scale.z = size.y;
		m_rotateTexture = rotateTexture;
	}

	Ceiling::Ceiling(glm::vec3 position, glm::vec2 size) // for stairs
	{
		m_transform.position = position;
		m_transform.scale.x = size.x;
		m_transform.scale.z = size.y;
		CalculateWorldSpaceCorners();
	}

	void Ceiling::Draw(Shader* shader)
	{
		GpuProfiler g("Ceiling");
		if (m_rotateTexture)
			shader->setInt("TEXTURE_FLAG", 3);
		else
			shader->setInt("TEXTURE_FLAG", 4);

		Util::DrawDownFacingPlane(shader, &m_transform);
		shader->setInt("TEXTURE_FLAG", 0);
	}

	void Ceiling::CalculateWorldSpaceCorners()
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