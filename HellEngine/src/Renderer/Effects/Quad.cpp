#include "hellpch.h"
#include "Quad.h"

namespace HellEngine
{
	Quad::Quad() 
	{
		float vertices[] = {
			-0.5f, +0.5f, +0.0f, 0.0f, 0.0f,
			+0.5f, +0.5f, +0.0f, 1.0f, 0.0f,
			+0.5f, -0.5f, +0.0f, 1.0f, 1.0f,
			-0.5f, -0.5f, +0.0f, 0.0f, 1.0f
		};
		uint32_t indices[] = { 0, 1, 2, 0, 2, 3 };

		glGenVertexArrays(1, &m_VAO);
		glBindVertexArray(m_VAO);

		glGenBuffers(1, &m_VBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(float)));

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glGenBuffers(1, &m_IBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glBindVertexArray(0);
	}

	void Quad::Draw()
	{
		glBindVertexArray(m_VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
}