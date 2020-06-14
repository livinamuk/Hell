#pragma once

#include "Renderer/Transform.h"
//#include "Platform/OpenGL/RenderableObject.h"
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

	public: // methods
		Decal(glm::vec3 position, glm::vec3 normal);
		void Draw(Shader* shader, bool blackOnly);

		static std::vector<Decal> s_decals;
	};
}