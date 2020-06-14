#pragma once
#include "header.h"

namespace HellEngine
{
	class Quad {
	public:
		Quad();
		void Draw();

	private:
		GLuint m_VAO;
		GLuint m_VBO;
		GLuint m_IBO;
	};
}