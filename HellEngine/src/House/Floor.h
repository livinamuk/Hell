#pragma once
#include "Header.h"

namespace HellEngine
{
	class Floor
	{
	public: // methods
		Floor();
		Floor(glm::vec3 position, glm::vec2 size, int story, bool rotateTexture);
		Floor(glm::vec3 position, glm::vec2 size); // for stairs
		void Draw(Shader* shader);
		void CalculateWorldSpaceCorners();

	public:	// fields
		Transform m_transform; 
		bool m_rotateTexture;
		std::vector<glm::vec3>worldSpaceCorners;
	};
}