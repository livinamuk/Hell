#pragma once

#include "Renderer/Transform.h"
#include "Header.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Renderer/Shader.h"

namespace HellEngine
{
	class Decal
	{
	public: // fields
		Transform transform;
	//	DecalType decalType;
		glm::vec3 normal;
		float randomRotation;
		DecalType m_type;

	public: // methods
		Decal(glm::vec3 position, glm::vec3 normal, DecalType type);
		void Draw(Shader* shader, bool blackOnly);

		static std::vector<Decal> s_decals;
	};
}