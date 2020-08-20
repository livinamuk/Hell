#include "hellpch.h"
#include "Util.h"
#include <Windows.h>

namespace HellEngine
{
	unsigned int Util::upFacingPlaneVAO = INVALID_VALUE;
	unsigned int Util::downFacingPlaneVAO = INVALID_VALUE;

	glm::vec3 orthogonal(glm::vec3 v)
	{
		float x = abs(v.x);
		float y = abs(v.y);
		float z = abs(v.z);

		glm::vec3 X_AXIS = glm::vec3(1, 0, 0);
		glm::vec3 Y_AXIS = glm::vec3(0, 1, 0);
		glm::vec3 Z_AXIS = glm::vec3(0, 0, 1);

		glm::vec3 other = x < y ? (x < z ? X_AXIS : Z_AXIS) : (y < z ? Y_AXIS : Z_AXIS);
		return cross(v, other);
	}

	void Util::RotateAxisBy90(Axis& axis)
	{ 
		int x = (int)axis;
		x++;
		x = x % 4;
		axis = (Axis)x;
	}

	char* Util::PhysicsObjectEnumToString(PhysicsObjectType objectType)
	{		
		if (objectType == PhysicsObjectType::UNDEFINED)
			return "UNDEFINED";
		if (objectType == PhysicsObjectType::WALL)
			return "WALL";
		if (objectType == PhysicsObjectType::FLOOR)
			return "FLOOR";
		if (objectType == PhysicsObjectType::DOOR)
			return "DOOR";
		if (objectType == PhysicsObjectType::STAIRS)
			return "STAIRS";
		if (objectType == PhysicsObjectType::PLAYER)
			return "PLAYER";
		if (objectType == PhysicsObjectType::RAGDOLL)
			return "RAGDOLL";
		if (objectType == PhysicsObjectType::CEILING)
			return "CEILING";
		if (objectType == PhysicsObjectType::SHELL_PROJECTILE)
			return "SHELL_PROJECTILE";
		if (objectType == PhysicsObjectType::MISC_MESH)
			return "MISC_MESH";
		if (objectType == PhysicsObjectType::EDITOR_WINDOW)
			return "WINDOW";
		if (objectType == PhysicsObjectType::GLASS)
			return "GLASS";
	}

	btQuaternion Util::GetQuatBetween2Points(glm::vec3 P0, glm::vec3 P1)
	{
		glm::vec3 u = glm::normalize(P1 - P0);
		glm::vec3 v = glm::vec3(0, 1, 0);

		glm::vec3 H = u + v;
		H = glm::normalize(H);

		float w = glm::dot(H, u);
		float x = u.y * H.z - u.z * H.y;
		float y = u.z * H.x - u.x * H.z;
		float z = u.x * H.y - u.y * H.x;
		
		return btQuaternion(0, 1, 0, 0) * btQuaternion(x, y, z, w);
	}

	const char* Util::CopyConstChar(const char* text)
	{
		char* b = new char[strlen(text) + 1]{};
		std::copy(text, text + strlen(text), b);
		return b;
	}

	void Util::InterpolateQuaternion(glm::quat& Out, const glm::quat& Start, const glm::quat& End, float pFactor)
	{
		// calc cosine theta
		float cosom = Start.x * End.x + Start.y * End.y + Start.z * End.z + Start.w * End.w;

		// adjust signs (if necessary)
		glm::quat end = End;
		if (cosom < static_cast<float>(0.0))
		{
			cosom = -cosom;
			end.x = -end.x;   // Reverse all signs
			end.y = -end.y;
			end.z = -end.z;
			end.w = -end.w;
		}

		// Calculate coefficients
		float sclp, sclq;
		if ((static_cast<float>(1.0) - cosom) > static_cast<float>(0.0001)) // 0.0001 -> some epsillon
		{
			// Standard case (slerp)
			float omega, sinom;
			omega = std::acos(cosom); // extract theta from dot product's cos theta
			sinom = std::sin(omega);
			sclp = std::sin((static_cast<float>(1.0) - pFactor) * omega) / sinom;
			sclq = std::sin(pFactor * omega) / sinom;
		}
		else
		{
			// Very close, do linear interp (because it's faster)
			sclp = static_cast<float>(1.0) - pFactor;
			sclq = pFactor;
		}

		Out.x = sclp * Start.x + sclq * end.x;
		Out.y = sclp * Start.y + sclq * end.y;
		Out.z = sclp * Start.z + sclq * end.z;
		Out.w = sclp * Start.w + sclq * end.w;
	}

	bool Util::StrCmp(const char* queryA, const char* queryB)
	{
		if (strcmp(queryA, queryB) == 0)
			return true;
		else 
			return false;
	}

	bool Util::CompareVertexPosition(Vertex v1, Vertex v2)
	{
		float eps = 0.00001;
		return ((fabs(v1.Position.x - v2.Position.x) < eps) && (fabs(v1.Position.y - v2.Position.y) > eps) && (fabs(v1.Position.z - v2.Position.z) > eps));
	}

	glm::mat4 Util::btScalar2mat4(btScalar* matrix) {
		return glm::mat4(
			matrix[0], matrix[1], matrix[2], matrix[3],
			matrix[4], matrix[5], matrix[6], matrix[7],
			matrix[8], matrix[9], matrix[10], matrix[11],
			matrix[12], matrix[13], matrix[14], matrix[15]);
	}

	glm::mat4 Util::Get_Mat4_From_Vec3(glm::vec3 position)
	{
		Transform trans;
		trans.position = position;
		return trans.to_mat4();
	}

	std::string Util::Mat4ToString(glm::mat4 m)
	{
		std::string s = "";
		s += "(" + std::to_string(m[0][0]) + ", " + std::to_string(m[0][1]) + ", " + std::to_string(m[0][2]) + ", " + std::to_string(m[0][3]) + ")\n";
		s += "(" + std::to_string(m[1][0]) + ", " + std::to_string(m[1][1]) + ", " + std::to_string(m[1][2]) + ", " + std::to_string(m[1][3]) + ")\n";
		s += "(" + std::to_string(m[2][0]) + ", " + std::to_string(m[2][1]) + ", " + std::to_string(m[2][2]) + ", " + std::to_string(m[2][3]) + ")\n";
		s += "(" + std::to_string(m[3][0]) + ", " + std::to_string(m[3][1]) + ", " + std::to_string(m[3][2]) + ", " + std::to_string(m[3][3]) + ")\n";
		return s;
	}

	void Util::TranslatePosition(glm::vec3* position, glm::mat4 translation)
	{
		glm::vec4 newPos = translation * glm::vec4(*position, 1.0f);
		*position = glm::vec3(newPos.x, newPos.y, newPos.z);
	}

	glm::vec3 Util::Position_From_Mat_4(glm::mat4 matrix)
	{
		glm::vec4 v = matrix * glm::vec4(0, 0, 0, 1.0f);
		return glm::vec3(v.x, v.y, v.z);
	}

	glm::vec3 Util::Vec3InterpTo(glm::vec3 Current, glm::vec3 Target, float DeltaTime, float InterpSpeed)
	{
		float x = FInterpTo(Current.x, Target.x, DeltaTime, InterpSpeed);
		float y = FInterpTo(Current.y, Target.y, DeltaTime, InterpSpeed);
		float z = FInterpTo(Current.z, Target.z, DeltaTime, InterpSpeed);
		return glm::vec3(x, y, z);
	}

	std::string Util::Vec3_to_String(glm::vec3 v)
	{
		return "(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ")";
	}

	float Util::FInterpTo(float Current, float Target, float DeltaTime, float InterpSpeed)
	{
		// If no interp speed, jump to target value
		if (InterpSpeed <= 0.f)
			return Target;

		// Distance to reach
		const float Dist = Target - Current;

		// If distance is too small, just set the desired location
		if (Util::Square(Dist) < SMALL_NUMBER)
			return Target;

		// Delta Move, Clamp so we do not over shoot.
		const float DeltaMove = Dist * std::clamp(DeltaTime * InterpSpeed, 0.0f, 1.0f);
		return Current + DeltaMove;
	}

	float Util::Square(float value)
	{
		return value * value;
	}

	glm::vec3 Util::ProjectVec3(glm::vec3 vector, const glm::vec3& other)
	{
		return (glm::dot(vector, other) / glm::length(other)) * other;
		//return (Dot(other) / other.MagnitudeSquared()) * other;
	}

	float Util::RandomFloat(float a, float b) {
		float random = ((float)rand()) / (float)RAND_MAX;
		float diff = b - a;
		float r = random * diff;
		return a + r;
	}

	void Util::PrintInRed(const char* text)
	{
		HANDLE  hConsole;
		int k;
		hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, 7);
		std::cout << text << "\n";
		SetConsoleTextAttribute(hConsole, 4);
	}

	btVector3 Util::glmVec3_to_btVec3(glm::vec3 vector)
	{
		return btVector3(vector.x, vector.y, vector.z);
	}

	btQuaternion Util::glmVec3_to_btQuat(glm::vec3 vector)
	{
		glm::quat r = glm::quat(vector);
		return btQuaternion(r.x, r.y, r.z, r.w);
	}

	glm::vec3 Util::btVec3_to_glmVec3(btVector3 vector)
	{
		return glm::vec3(vector.x(), vector.y(), vector.z());
	}


	bool Util::StringContainsString(const char* string, const char* query)
	{
		for (size_t i = 0; i < strlen(string); i++)
		{
			for (size_t j = 0; j < strlen(query); j++)
			{
				if (query[j] != string[i + j])
					break;
				if (j == strlen(query) - 1)
					return true;
			}
		}
		return false;
	}

	std::string Util::RemoveCharactersFromEnd(std::string string, int number)
	{
		std::string result = string.substr(0, string.length() - number);
		return result;
	}

	std::string Util::RemoveCharactersFromBeginning(std::string string, int number)
	{
		std::string result = string.substr(number, string.length() - number);
		return result;
	}

	bool Util::StringEndsIn(std::string const& fullString, std::string const& ending) {
		if (fullString.length() >= ending.length()) {
			return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
		}
		else {
			return false;
		}
	}


	void Util::DrawDownFacingPlane(Shader* shader, Transform* transform)
	{
		// Setup if you haven't already
		if (downFacingPlaneVAO == INVALID_VALUE) {
			Vertex vert0, vert1, vert2, vert3;
			vert0.Position = glm::vec3(-0.5, 0, 0.5);
			vert1.Position = glm::vec3(-0.5, 0, -0.5f);
			vert2.Position = glm::vec3(0.5, 0, 0.5);
			vert3.Position = glm::vec3(0.5, 0, -0.5);
			vert0.Normal = glm::vec3(0, -1, 0);
			vert1.Normal = glm::vec3(0, -1, 0);
			vert2.Normal = glm::vec3(0, -1, 0);
			vert3.Normal = glm::vec3(0, -1, 0);
			vert0.TexCoords = glm::vec2(0, 0);
			vert1.TexCoords = glm::vec2(1, 0);
			vert2.TexCoords = glm::vec2(1, 1);
			vert3.TexCoords = glm::vec2(0, 1);
			Util::SetTangentsFromVertices(&vert0, &vert1, &vert2);
			Util::SetTangentsFromVertices(&vert3, &vert0, &vert1);

			/*
			vert0.Normal = glm::vec3(0, -1, 0);
			vert1.Normal = glm::vec3(0, -1, 0);
			vert2.Normal = glm::vec3(0, -1, 0);
			vert3.Normal = glm::vec3(0, -1, 0);
			Util::SetTangentsFromVertices(&vert0, &vert1, &vert2);
			Util::SetTangentsFromVertices(&vert3, &vert0, &vert1);
			*/
			std::vector<Vertex> vertices;
			std::vector<unsigned int> indices;
			unsigned int i = vertices.size();
			indices.push_back(0);
			indices.push_back(1);
			indices.push_back(2);
			indices.push_back(1);
			indices.push_back(3);
			indices.push_back(2);
			vertices.push_back(vert0);
			vertices.push_back(vert1);
			vertices.push_back(vert2);
			vertices.push_back(vert3);
			unsigned int VBO;
			unsigned int EBO;
			glGenVertexArrays(1, &downFacingPlaneVAO);
			glGenBuffers(1, &VBO);
			glGenBuffers(1, &EBO);
			glBindVertexArray(downFacingPlaneVAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
		}
		// Draw
		glBindVertexArray(downFacingPlaneVAO);
		shader->setMat4("model", transform->to_mat4());
		glDrawElements(GL_TRIANGLES, 8, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	void Util::DrawUpFacingPlane(Shader* shader, Transform* transform)
	{
		// Setup if you haven't already
		if (upFacingPlaneVAO == INVALID_VALUE) {
			Vertex vert0, vert1, vert2, vert3;
			vert0.Position = glm::vec3(-0.5, 0, 0.5);
			vert1.Position = glm::vec3(0.5, 0, 0.5f);
			vert2.Position = glm::vec3(0.5, 0, -0.5);
			vert3.Position = glm::vec3(-0.5, 0, -0.5);
			vert0.TexCoords = glm::vec2(0, 0);
			vert1.TexCoords = glm::vec2(1, 0);
			vert2.TexCoords = glm::vec2(1, 1);
			vert3.TexCoords = glm::vec2(0, 1);
			Util::SetNormalsAndTangentsFromVertices(&vert0, &vert1, &vert2);
			Util::SetNormalsAndTangentsFromVertices(&vert3, &vert0, &vert1);
			std::vector<Vertex> vertices;
			std::vector<unsigned int> indices;
			unsigned int i = vertices.size();
			indices.push_back(i);
			indices.push_back(i + 1);
			indices.push_back(i + 2);
			indices.push_back(i + 2);
			indices.push_back(i + 3);
			indices.push_back(i);
			vertices.push_back(vert0);
			vertices.push_back(vert1);
			vertices.push_back(vert2);
			vertices.push_back(vert3);
			unsigned int VBO;
			unsigned int EBO;
			glGenVertexArrays(1, &upFacingPlaneVAO);
			glGenBuffers(1, &VBO);
			glGenBuffers(1, &EBO);
			glBindVertexArray(upFacingPlaneVAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
		}
		// Draw
		glBindVertexArray(upFacingPlaneVAO);
		shader->setMat4("model", transform->to_mat4());
		glDrawElements(GL_TRIANGLES, 8, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	void Util::SetTangentsFromVertices(Vertex* vert0, Vertex* vert1, Vertex* vert2)
	{
		// Shortcuts for UVs
		glm::vec3& v0 = vert0->Position;
		glm::vec3& v1 = vert1->Position;
		glm::vec3& v2 = vert2->Position;
		glm::vec2& uv0 = vert0->TexCoords;
		glm::vec2& uv1 = vert1->TexCoords;
		glm::vec2& uv2 = vert2->TexCoords;

		// Edges of the triangle : postion delta. UV delta
		glm::vec3 deltaPos1 = v1 - v0;
		glm::vec3 deltaPos2 = v2 - v0;
		glm::vec2 deltaUV1 = uv1 - uv0;
		glm::vec2 deltaUV2 = uv2 - uv0;

		float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
		glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

		vert0->Tangent = tangent;
		vert1->Tangent = tangent;
		vert2->Tangent = tangent;

		vert0->Bitangent = bitangent;
		vert1->Bitangent = bitangent;
		vert2->Bitangent = bitangent;
	}

	void Util::SetNormalsAndTangentsFromVertices(Vertex* vert0, Vertex* vert1, Vertex* vert2)
	{
		// above can be used to replace the shit below here. its the same.

		// Shortcuts for UVs
		glm::vec3& v0 = vert0->Position;
		glm::vec3& v1 = vert1->Position;
		glm::vec3& v2 = vert2->Position;
		glm::vec2& uv0 = vert0->TexCoords;
		glm::vec2& uv1 = vert1->TexCoords;
		glm::vec2& uv2 = vert2->TexCoords;

		// Edges of the triangle : postion delta. UV delta
		glm::vec3 deltaPos1 = v1 - v0;
		glm::vec3 deltaPos2 = v2 - v0;
		glm::vec2 deltaUV1 = uv1 - uv0;
		glm::vec2 deltaUV2 = uv2 - uv0;

		float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
		glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

		glm::vec3 normal = Util::NormalFromTriangle(vert0->Position, vert1->Position, vert2->Position);

		vert0->Normal = normal;
		vert1->Normal = normal;
		vert2->Normal = normal;

		vert0->Tangent = tangent;
		vert1->Tangent = tangent;
		vert2->Tangent = tangent;

		vert0->Bitangent = bitangent;
		vert1->Bitangent = bitangent;
		vert2->Bitangent = bitangent;
	}

	glm::vec3 Util::NormalFromTriangle(glm::vec3 pos0, glm::vec3 pos1, glm::vec3 pos2)
	{
		return glm::normalize(glm::cross(pos1 - pos0, pos2 - pos0));
	}

	glm::vec3 Util::SetRotationByAxis(Axis axis)
	{
		glm::vec3 rot;
		float amount;
		if (axis == Axis::POS_X)
			amount = 0;
		else if (axis == Axis::NEG_X)
			amount = ROTATE_180;
		else if (axis == Axis::POS_Z)
			amount = ROTATE_90;
		else
			amount = ROTATE_270;

		return glm::vec3(0, amount, 0);
	}
	std::string Util::FileNameFromPath(std::string filepath)
	{
		// Remove directory if present.
		// Do this before extension removal incase directory has a period character.
		const size_t last_slash_idx = filepath.find_last_of("\\/");
		if (std::string::npos != last_slash_idx)
		{
			filepath.erase(0, last_slash_idx + 1);
		}

		// Remove extension if present.
		const size_t period_idx = filepath.rfind('.');
		if (std::string::npos != period_idx)
		{
			filepath.erase(period_idx);
		}
		return filepath;
	}
	FileType Util::FileTypeFromPath(std::string filepath)
	{
		int index = filepath.find_last_of('.') + 1;
		std::string extension = filepath.substr(index);
		//std::cout << "'" << extension << "'\n";

		if (extension == "PNG" || extension == "png")
			return FileType::PNG;
		if (extension == "JPG" || extension == "jpg")
			return FileType::JPG;
		if (extension == "FBX" || extension == "fbx")
			return FileType::FBX;
		if (extension == "OBJ" || extension == "obj")
			return FileType::OBJ;
		else
			return FileType::UNKNOWN;
	}

	glm::vec3 Util::GetTranslationFromMatrix(glm::mat4 matrix)
	{
	/*	glm::vec3 scale;
		glm::quat rotation;
		glm::vec3 translation;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(matrix, scale, rotation, translation, skew, perspective);
		return translation * scale;*/

		/*glm::vec4 result = glm::vec4(0, 0, 0, 1) * matrix;
		return glm::vec3(result.x, result.y, result.z);*/

		return glm::vec3(matrix[3][0], matrix[3][1], matrix[3][2]);
	}

	void Util::DrawPoint(Shader* shader, glm::vec3 position, glm::mat4 modelMatrix, glm::vec3 color)
	{
		shader->setMat4("model", modelMatrix);
		Vertex vert0;
		vert0.Position = position;
		vert0.Normal = color;	// Yes. You are storing the color in the vertex normal spot.

		unsigned int VAO, VBO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex), &vert0, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

		glPointSize(4.0);
		glBindVertexArray(VAO);
		glDrawArrays(GL_POINTS, 0, 1);

		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
	}


	glm::mat4 QuarternionToRotationMatrix(glm::quat q)
	{
		glm::mat4 matrix = glm::mat4(0);
		float xy = q.x * q.y;
		float xz = q.x * q.z;
		float xw = q.x * q.w;
		float yz = q.y * q.z;
		float yw = q.y * q.w;
		float zw = q.z * q.w;
		float xSquared = q.x * q.x;
		float ySquared = q.y * q.y;
		float zSquared = q.z * q.z;
		matrix[0][0] = 1 - 2 * (ySquared + zSquared);
		matrix[0][1] = 2 * (xy - zw);
		matrix[0][2] = 2 * (xz + yw);
		matrix[0][4] = 0;
		matrix[1][0] = 2 * (xy + zw);
		matrix[1][1] = 1 - 2 * (xSquared + zSquared);
		matrix[1][2] = 2 * (yz - xw);
		matrix[1][3] = 0;
		matrix[2][0] = 2 * (xz - yw);
		matrix[2][1] = 2 * (yz + xw);
		matrix[2][2] = 1 - 2 * (xSquared + ySquared);
		matrix[2][3] = 0;
		matrix[3][0] = 0;
		matrix[3][1] = 0;
		matrix[3][2] = 0;
		matrix[3][3] = 1;
		return matrix;
	}

	glm::mat4 Util::Mat4InitScaleTransform(float ScaleX, float ScaleY, float ScaleZ)
	{
	/*	glm::mat4 m = glm::mat4(1);
		m[0][0] = ScaleX; m[0][1] = 0.0f;   m[0][2] = 0.0f;   m[0][3] = 0.0f;
		m[1][0] = 0.0f;   m[1][1] = ScaleY; m[1][2] = 0.0f;   m[1][3] = 0.0f;
		m[2][0] = 0.0f;   m[2][1] = 0.0f;   m[2][2] = ScaleZ; m[2][3] = 0.0f;
		m[3][0] = 0.0f;   m[3][1] = 0.0f;   m[3][2] = 0.0f;   m[3][3] = 1.0f;
		return m;*/

		return glm::scale(glm::mat4(1.0), glm::vec3(ScaleX, ScaleY, ScaleZ));
	}

	glm::mat4 Util::Mat4InitRotateTransform(float RotateX, float RotateY, float RotateZ)
	{
		glm::mat4 rx = glm::mat4(1);
		glm::mat4 ry = glm::mat4(1);
		glm::mat4 rz = glm::mat4(1);

		const float x = ToRadian(RotateX);
		const float y = ToRadian(RotateY);
		const float z = ToRadian(RotateZ);

		rx[0][0] = 1.0f; rx[0][1] = 0.0f; rx[0][2] = 0.0f; rx[0][3] = 0.0f;
		rx[1][0] = 0.0f; rx[1][1] = cosf(x); rx[1][2] = -sinf(x); rx[1][3] = 0.0f;
		rx[2][0] = 0.0f; rx[2][1] = sinf(x); rx[2][2] = cosf(x); rx[2][3] = 0.0f;
		rx[3][0] = 0.0f; rx[3][1] = 0.0f; rx[3][2] = 0.0f; rx[3][3] = 1.0f;

		ry[0][0] = cosf(y); ry[0][1] = 0.0f; ry[0][2] = -sinf(y); ry[0][3] = 0.0f;
		ry[1][0] = 0.0f; ry[1][1] = 1.0f; ry[1][2] = 0.0f; ry[1][3] = 0.0f;
		ry[2][0] = sinf(y); ry[2][1] = 0.0f; ry[2][2] = cosf(y); ry[2][3] = 0.0f;
		ry[3][0] = 0.0f; ry[3][1] = 0.0f; ry[3][2] = 0.0f; ry[3][3] = 1.0f;

		rz[0][0] = cosf(z); rz[0][1] = -sinf(z); rz[0][2] = 0.0f; rz[0][3] = 0.0f;
		rz[1][0] = sinf(z); rz[1][1] = cosf(z); rz[1][2] = 0.0f; rz[1][3] = 0.0f;
		rz[2][0] = 0.0f; rz[2][1] = 0.0f; rz[2][2] = 1.0f; rz[2][3] = 0.0f;
		rz[3][0] = 0.0f; rz[3][1] = 0.0f; rz[3][2] = 0.0f; rz[3][3] = 1.0f;

		return rz * ry * rx;
	}

	glm::mat4 Util::Mat4InitTranslationTransform(float x, float y, float z)
	{/*
		glm::mat4 m = glm::mat4(1);
		m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = x;
		m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = y;
		m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = z;
		m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
		return m;
		*/
		return  glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
	}

	glm::mat4 Util::aiMatrix4x4ToGlm(const aiMatrix4x4& from)
	{
		/*glm::mat4 to;
		//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
		to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
		to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
		to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
		to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
		return to;*/

		glm::mat4 to;
		//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
		to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
		to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
		to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
		to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
		return to;
	}

	glm::mat4 Util::aiMatrix3x3ToGlm(const aiMatrix3x3& from)
	{
		glm::mat4 to;
		to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = 0.0;
		to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = 0.0;
		to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = 0.0;
		to[0][3] = 0.0; to[1][3] = 0.0; to[2][3] = 0.0; to[3][3] = 1.0;
		return to;
	}

	/*void Util::RemoveTranslation(glm::mat4& matrix)
	{
		//glm::mat4 to;
		//to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = 0.0;
		//to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = 0.0;
		//to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = 0.0;
		matrix[3][0] = 0.0f; matrix[3][1] = 0.0; matrix[3][2] = 0.0;// to[3][3] = 1.0;
		//return to;
	}
	*/
	void Util::PrintMat4(glm::mat4 m)
	{
		std::cout << "(" << m[0][0] << ", " << m[0][1] << ", " << m[0][2] << ", " << m[0][3] << ")\n";
		std::cout << "(" << m[1][0] << ", " << m[1][1] << ", " << m[1][2] << ", " << m[1][3] << ")\n";
		std::cout << "(" << m[2][0] << ", " << m[2][1] << ", " << m[2][2] << ", " << m[2][3] << ")\n";
		std::cout << "(" << m[3][0] << ", " << m[3][1] << ", " << m[3][2] << ", " << m[3][3] << ")\n";
	}

	std::string Util::FileTypeToString(FileType fileType)
	{
		if (fileType == FileType::FBX)
			return "FBX";
		if (fileType == FileType::OBJ)
			return "OBJ";
		if (fileType == FileType::PNG)
			return "PNG";
		if (fileType == FileType::JPG)
			return "JPG";
		else
			return "UNKNOWN";
	}

	std::string Util::AxisToString(Axis axis)
	{
		if (axis == Axis::POS_X)
			return "POS_X";
		if (axis == Axis::POS_Z)
			return "POS_Z";
		if (axis == Axis::NEG_X)
			return "NEG_X";
		if (axis == Axis::NEG_Z)
			return "NEG_Z";
		else
			return "INVALID AXIS";
	}

	Axis Util::StringToAxis(std::string string)
	{
		if (string == "POS_X")
			return Axis::POS_X;
		if (string == "POS_Z")
			return Axis::POS_Z;
		if (string == "NEG_X")
			return Axis::NEG_X;
		else
			return Axis::NEG_Z;
	}

	unsigned int Util::CountLines(std::string str)
	{
		long count = 0;
		int position = 0;
		while ((position = str.find('\n', position)) != -1)
		{
			count++;
			position++;         // Skip this occurrence!
		}
		return count;
	}

	void Util::EraseFirstLine(std::string* str)
	{
		str->erase(0, str->find("\n") + 1);
	}   
	
	Derivative Util::evaluate(const State& initial,	double t, float dt, const Derivative& d)
	{
		State state;
		state.x = initial.x + d.dx * dt;
		state.v = initial.v + d.dv * dt;

		Derivative output;
		output.dx = state.v;
		output.dv = acceleration(state, t + dt);
		return output;
	}

	float Util::acceleration(const State& state, double t)
	{
		const float k = 15.0f;
		const float b = 0.1f;
		return -k * state.x - b * state.v;
	}

	void Util::integrate(State& state, double t, float dt)
	{
		Derivative a, b, c, d;

		a = evaluate(state, t, 0.0f, Derivative());
		b = evaluate(state, t, dt * 0.5f, a);
		c = evaluate(state, t, dt * 0.5f, b);
		d = evaluate(state, t, dt, c);

		float dxdt = 1.0f / 6.0f *
			(a.dx + 2.0f * (b.dx + c.dx) + d.dx);

		float dvdt = 1.0f / 6.0f *
			(a.dv + 2.0f * (b.dv + c.dv) + d.dv);

		state.x = state.x + dxdt * dt;
		state.v = state.v + dvdt * dt;
	}
	glm::vec3 Util::TranslationFromMat4(glm::mat4& matrix)
	{
		float x = matrix[3][0];
		float y = matrix[3][1];
		float z = matrix[3][2];
		return glm::vec3(x, y, z);
	}

	btVector3 Util::GetRelPosBetween2Vectors(glm::vec3 vecA, glm::vec3 vecB)
	{
		glm::vec3 relativePos = (vecA + vecB) * glm::vec3(0.5f);
		return Util::glmVec3_to_btVec3(relativePos);
	}

	btQuaternion Util::GetRotationFromBoneMatrix(glm::mat4 matrix)
	{
		glm::quat q = glm::quat_cast(matrix);
		return btQuaternion(q.x, q.y, q.z, q.w) * btQuaternion(0.70710678118, -0.70710678118, 0, 0);
	}

	glm::mat4 Util::FlipAxis(glm::mat4& matrix)
	{
		glm::mat4 flip;
		flip[0][0] = 1; flip[1][0] = 0; flip[2][0] = 0; flip[3][0] = 0;
		flip[0][1] = 0; flip[1][1] = 0; flip[2][1] = 1; flip[3][1] = 0;
		flip[0][2] = 0; flip[1][2] =-1; flip[2][2] = 0; flip[3][2] = 0;
		flip[0][3] = 0; flip[1][3] = 0; flip[2][3] = 0; flip[3][3] = 1;

		return flip * matrix;
	}

	glm::mat4 Util::btTransformToMat4(btTransform& trans) // Ignores scale.
	{
		btVector3 pos = trans.getOrigin();
		btQuaternion rot = trans.getRotation();
		glm::vec3 posGL = glm::vec3(pos.x(), pos.y(), pos.z());
		glm::quat rotQL = glm::quat(rot.w(), rot.x(), rot.y(), rot.z());
		glm::mat4 m = glm::translate(glm::mat4(1), posGL);
		m *= glm::mat4_cast(rotQL);
		return  m;
	}
}