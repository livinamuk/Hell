#include "hellpch.h"
#include "LightVolumeDoorWay.h"
#include "Helpers/Util.h"
#include "GpuProfiling.h"

namespace HellEngine
{
	LightVolumeDoorWay::LightVolumeDoorWay(HoleInWall& doorWay, glm::vec3 lightPosition, float lightRadius, float roomEdge)
	{
		this->VAO = 0;
		this->m_type = doorWay.type;

		// Store a pointer to the partent door. This is used for skipping drawing the light volume if the door is closed.
		if (doorWay.type == HoleInWallType::DOOR)
			this->p_parentDoor = (Door*)doorWay.parent;

		glm::vec3 doorHoleBottomLeft;
		glm::vec3 doorHoleBottomRight;
		glm::vec3 doorHoleTopLeft;
		glm::vec3 doorHoleTopRight;
		float bias = 0.02f;

		if (doorWay.axis == Axis::POS_X || doorWay.axis == Axis::NEG_X)
		{
			doorHoleBottomLeft = glm::vec3(-0.4 + doorWay.position.x, -bias, roomEdge);
			doorHoleBottomRight = glm::vec3(0.4 + doorWay.position.x, -bias, roomEdge);
			doorHoleTopLeft = glm::vec3(-0.4 + doorWay.position.x, 2, roomEdge);
			doorHoleTopRight = glm::vec3(0.4 + doorWay.position.x, 2, roomEdge);
		}
		else
		{
			doorHoleBottomLeft = glm::vec3(roomEdge , -bias, -0.4 + doorWay.position.z);
			doorHoleBottomRight = glm::vec3(roomEdge, -bias, 0.4 + doorWay.position.z);
			doorHoleTopLeft = glm::vec3(roomEdge , 2, -0.4 + doorWay.position.z);
			doorHoleTopRight = glm::vec3(roomEdge, 2, 0.4 + doorWay.position.z);
		}

		//Util::TranslatePosition(&doorHoleBottomLeft, Transform(doorWay.position, Util::SetRotationByAxis(doorWay.axis)).to_mat4());
		//Util::TranslatePosition(&doorHoleBottomRight, Transform(doorWay.position, Util::SetRotationByAxis(doorWay.axis)).to_mat4());
		//Util::TranslatePosition(&doorHoleTopLeft, Transform(doorWay.position, Util::SetRotationByAxis(doorWay.axis)).to_mat4());
		//Util::TranslatePosition(&doorHoleTopRight, Transform(doorWay.position, Util::SetRotationByAxis(doorWay.axis)).to_mat4());

		glm::vec3 origin = lightPosition;

		glm::vec3 direction = normalize(doorHoleBottomLeft - origin);
		glm::vec3 A = origin + (direction * lightRadius);
		direction = normalize(doorHoleBottomRight - origin);
		glm::vec3 B = origin + (direction * lightRadius);
		direction = normalize(doorHoleTopLeft - origin);
		glm::vec3 D = origin + (direction * lightRadius);
		direction = normalize(doorHoleTopRight - origin);
		glm::vec3 C = origin + (direction * lightRadius);

		// fix limits
	//	A.y = std::max(A.y, floorHeight - bias);
	//	B.y = std::max(B.y, floorHeight - bias);

		// Extend bottom to match the reach of the top. Hopefully the sphere still includes this??
		//A.x = C.x;
		//B.z = D.z;
		// Nope it isn't that simple. But have a think here, it's doable.

		// Bottom
		vertices.push_back(B);
		vertices.push_back(A);
		vertices.push_back(doorHoleBottomLeft);

		vertices.push_back(B);
		vertices.push_back(doorHoleBottomLeft);
		vertices.push_back(doorHoleBottomRight);

		// Left side
		vertices.push_back(doorHoleTopLeft);
		vertices.push_back(doorHoleBottomLeft);
		vertices.push_back(A);

		vertices.push_back(doorHoleTopLeft);
		vertices.push_back(A);
		vertices.push_back(D);

		// Right side
		vertices.push_back(B);
		vertices.push_back(doorHoleBottomRight);
		vertices.push_back(doorHoleTopRight);

		vertices.push_back(C);
		vertices.push_back(B);
		vertices.push_back(doorHoleTopRight);

		// Endcap
		vertices.push_back(A);
		vertices.push_back(B);
		vertices.push_back(C);

		vertices.push_back(C);
		vertices.push_back(D);
		vertices.push_back(A);

		// Front cap (towards room)
		vertices.push_back(doorHoleBottomRight);
		vertices.push_back(doorHoleBottomLeft);
		vertices.push_back(doorHoleTopRight);

		vertices.push_back(doorHoleBottomLeft);
		vertices.push_back(doorHoleTopLeft);
		vertices.push_back(doorHoleTopRight);

		// Top
		vertices.push_back(doorHoleTopRight);
		vertices.push_back(doorHoleTopLeft);
		vertices.push_back(C);

		vertices.push_back(doorHoleTopLeft);
		vertices.push_back(D);
		vertices.push_back(C);

	//	std::cout << "vertices: " << vertices.size() << "\n";

		// RETURN!
		Setup();
	}

	void LightVolumeDoorWay::Draw(Shader* shader)
	{
		GpuProfiler g("LightVolumeDoorway");
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		shader->setMat4("model", modelMatrix);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	//	glPointSize(4);
	//	glDrawArrays(GL_POINTS, 0, vertices.size());

		//std::cout << "DRAWING LUIGHT DOORWAY\n";
	}

	void LightVolumeDoorWay::Setup()
	{
		unsigned int VBO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
		glBindVertexArray(VAO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	}
}