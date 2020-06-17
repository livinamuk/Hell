#include "hellpch.h"
#include "Transform.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/common.hpp>
#include <math.h>


glm::mat4 Transform::to_mat4()
{
	/*glm::mat4 m = glm::translate(glm::mat4(1), position);
	m = glm::rotate(m, rotation.z, glm::vec3(0, 0, 1));
	m = glm::rotate(m, rotation.y, glm::vec3(0, 1, 0));
	m = glm::rotate(m, rotation.x, glm::vec3(1, 0, 0));
	m = glm::scale(m, scale);*/

	glm::mat4 m = glm::translate(glm::mat4(1), position);
	glm::quat qt = glm::quat(rotation);
	m *= glm::mat4_cast(qt);
	m = glm::scale(m, scale);
	return m;

	

	
//	glm::mat4 TranslationMatrix = glm::translate(glm::mat4(1), position);
//	glm::mat4 ScaleMatrix = glm::scale(glm::mat4(1), scale);

/*	float pYaw = rotation.y;
	float pRoll = rotation.z;
	float pPitch = rotation.x;

	float t0 = std::cos(pYaw * 0.5);
	float t1 = std::sin(pYaw * 0.5);
	float t2 = std::cos(pRoll * 0.5);
	float t3 = std::sin(pRoll * 0.5);
	float t4 = std::cos(pPitch * 0.5);
	float t5 = std::sin(pPitch * 0.5);

	float w = t0 * t2 * t4 + t1 * t3 * t5;
	float x = t0 * t3 * t4 - t1 * t2 * t5;
	float y = t0 * t2 * t5 + t1 * t3 * t4;
	float z = t1 * t2 * t4 - t0 * t3 * t5;

	glm::quat q = glm::quat(w, x, y, z);

	*/

	//glm::quat rotY = glm::angleAxis(glm::radians(rotation.y), glm::vec3(0, 1, 0));
/*	glm::quat rotX = glm::angleAxis(rotation.x, glm::vec3(1, 0, 0));
	glm::quat rotY = glm::angleAxis(rotation.y, glm::vec3(0, 1, 0));
	glm::quat rotZ = glm::angleAxis(rotation.z, glm::vec3(0, 0, 1));

	glm::mat4 matX = glm::mat4_cast(rotX);
	glm::mat4 matY = glm::mat4_cast(rotY);
	glm::mat4 matZ = glm::mat4_cast(rotZ);

	return TranslationMatrix * matX * matY * matZ * ScaleMatrix;*/

	/*

	float x = sin(RotationAngle / 2);
	float y = RotationAxis.y * sin(RotationAngle / 2);
	float z = RotationAxis.z * sin(RotationAngle / 2);
	float w = cos(RotationAngle / 2);*/
		;
	//glm::quat MyQuaternion = gtx::quaternion::angleAxis(degrees(RotationAngle), RotationAxis);
	
	glm::quat myquaternion = glm::quat(glm::vec3(rotation.z, rotation.y, rotation.x));
	//glm::mat4 RotationMatrix = glm::mat4_cast(myquaternion);

	//return TranslationMatrix * ScaleMatrix * RotationMatrix;

	//return m;
}

glm::mat4 Transform::blood_mat4()
{
	
	/*


		//glm::quat rotY = glm::angleAxis(glm::radians(rotation.y), glm::vec3(0, 1, 0));
	glm::quat rotX = glm::angleAxis(rotation.x, glm::vec3(1, 0, 0));
	glm::quat rotY = glm::angleAxis(rotation.y, glm::vec3(0, 1, 0));
	glm::quat rotZ = glm::angleAxis(rotation.z, glm::vec3(0, 0, 1));

	glm::mat4 matX = glm::mat4_cast(rotX);
	glm::mat4 matY = glm::mat4_cast(rotY);
	glm::mat4 matZ = glm::mat4_cast(rotZ);

	return TranslationMatrix * matZ * matY * matX * ScaleMatrix; */
	
	glm::mat4 m = glm::translate(glm::mat4(1), position);
	m = glm::rotate(m, rotation.y, glm::vec3(0, 1, 0));
	m = glm::rotate(m, rotation.x, glm::vec3(1, 0, 0));
	m = glm::rotate(m, rotation.z, glm::vec3(0, 0, 1));
	m = glm::scale(m, scale);
	return m;
}

Transform::Transform()
{
}


Transform::Transform(glm::mat4 transformation)
{
	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(transformation, scale, rotation, translation, skew, perspective);
	
	rotation = glm::conjugate(rotation);

	Transform t;
	t.position = translation;
	glm::extractEulerAngleZYX(transformation, t.rotation.x, t.rotation.y, t.rotation.z);
	t.scale = scale;
}

Transform::Transform(glm::vec3 position)
{
	this->position = position;
}

Transform::Transform(glm::vec3 position, glm::vec3 rotation)
{
	this->position = position;
	this->rotation = rotation;
}

Transform::Transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
	this->position = position;
	this->rotation = rotation;
	this->scale = scale;
}