#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "Transform.h"

class Cube
{
	public: // methods
		Cube(glm::vec3 position);
		Cube(glm::vec3 position, glm::vec3 scale);
		void Draw(Shader* shader);

	public: // fields
		Transform m_transform;
		static unsigned int VAO;
};
