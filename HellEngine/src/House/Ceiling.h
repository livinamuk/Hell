#pragma once
#include "Header.h"

namespace HellEngine
{
	class Ceiling
	{
	public: // methods
		Ceiling();
		Ceiling(glm::vec3 position, glm::vec2 size, int story, bool rotateTexture);
		Ceiling(glm::vec3 position, glm::vec2 size);
		void Draw(Shader* shader);
		void CalculateWorldSpaceCorners();

	public:	// fields
		Transform m_transform;
		bool m_rotateTexture;
		std::vector<glm::vec3>worldSpaceCorners;
	};
}