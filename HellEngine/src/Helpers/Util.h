#pragma once
#include "Header.h"
#include "Renderer/Transform.h"
#include "Renderer/Shader.h"

#include "bullet/src/btBulletCollisionCommon.h"
#include "bullet/src/btBulletDynamicsCommon.h"
#include "bullet/src/LinearMath/btVector3.h"
#include "bullet/src/LinearMath/btAlignedObjectArray.h"

#include "assimp/matrix4x4.h"
#include "assimp/matrix3x3.h"

namespace HellEngine
{
	class Util
	{
	public:
		static btQuaternion GetQuatBetween2Points(glm::vec3 a, glm::vec3 b);
		static const char* CopyConstChar(const char* text);;
		static void InterpolateQuaternion(glm::quat& pOut, const glm::quat& Start, const glm::quat& End, float Factor);
	//	static void RemoveTranslation(glm::mat4& matrix);
		static bool StrCmp(const char* queryA, const char* queryB);
		static bool CompareVertexPosition(Vertex v1, Vertex v2);
		static glm::mat4 btScalar2mat4(btScalar* matrix);
		static glm::mat4 Get_Mat4_From_Vec3(glm::vec3 position);
		static void TranslatePosition(glm::vec3* position, glm::mat4 translation);
		static glm::vec3 Position_From_Mat_4(glm::mat4 matrix);
		static float Square(float value);
		static float FInterpTo(float Current, float Target, float DeltaTime, float InterpSpeed);
		static glm::vec3 Vec3InterpTo(glm::vec3 Current, glm::vec3 Target, float DeltaTime, float InterpSpeed); 
		static glm::vec3 ProjectVec3(glm::vec3 vector, const glm::vec3& other); // Projection of this on to other
		static bool StringContainsString(const char* string, const char* query); 
		static std::string RemoveCharactersFromEnd(std::string string, int number);
		static std::string RemoveCharactersFromBeginning(std::string string, int number);
		static bool Util::StringEndsIn(std::string const& fullString, std::string const& ending);		
		static void DrawUpFacingPlane(Shader* shader, Transform* transform);
		static void DrawDownFacingPlane(Shader* shader, Transform* transform);
		static void SetNormalsAndTangentsFromVertices(Vertex* vert0, Vertex* vert1, Vertex* vert2);
		static void SetTangentsFromVertices(Vertex* vert0, Vertex* vert1, Vertex* vert2);
		static glm::vec3 Util::NormalFromTriangle(glm::vec3 pos0, glm::vec3 pos1, glm::vec3 pos2);
		static glm::vec3 SetRotationByAxis(Axis axis);
		static std::string FileNameFromPath(std::string filepath);
		static FileType FileTypeFromPath(std::string filepath);
		static btVector3 glmVec3_to_btVec3(glm::vec3);
		static glm::vec3 btVec3_to_glmVec3(btVector3);
		static std::string Vec3_to_String(glm::vec3 vector);

		static std::string FileTypeToString(FileType fileType);
		static std::string AxisToString(Axis axis);
		static Axis StringToAxis(std::string string);

		static glm::vec3 GetTranslationFromMatrix(glm::mat4 matrix);
		static void DrawPoint(Shader* shader, glm::vec3 position, glm::mat4 modelMatrix, glm::vec3 color);
		static float Util::RandomFloat(float a, float b);
		static void PrintInRed(const char* text);

		static glm::mat4 EmptyMat4();
		static glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4& from);
		static glm::mat4 aiMatrix3x3ToGlm(const aiMatrix3x3& from);
		static glm::mat4 Mat4InitScaleTransform(float ScaleX, float ScaleY, float ScaleZ);
		static glm::mat4 Mat4InitRotateTransform(float RotateX, float RotateY, float RotateZ);
		static glm::mat4 Mat4InitTranslationTransform(float x, float y, float z);
		static std::string Mat4ToString(glm::mat4 m);
		static void PrintMat4(glm::mat4 m);
		static unsigned int CountLines(std::string str);
		static void EraseFirstLine(std::string* str);

		static Derivative evaluate(const State& initial, double t, float dt, const Derivative& d);
		static float acceleration(const State& state, double t);
		static void integrate(State& state, double t, float dt);

		static glm::vec3 TranslationFromMat4(glm::mat4& matrix);

		static btVector3 GetRelPosBetween2Vectors(glm::vec3 vecA, glm::vec3 vecB);
		static btQuaternion GetRotationFromBoneMatrix(glm::mat4 matrix);

		static glm::mat4 FlipAxis(glm::mat4& matrix);

		static glm::mat4 btTransformToMat4(btTransform& trans);
	
	private:
			static unsigned int upFacingPlaneVAO;
			static unsigned int downFacingPlaneVAO;
	};
}