#include "hellpch.h"
#include "Transform.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/common.hpp>
#include <math.h>


glm::mat4 Transform::to_mat4()
{
	glm::mat4 m = glm::translate(glm::mat4(1), position);
	
	glm::quat qt = glm::quat(rotation);
	m *= glm::mat4_cast(qt);

	m = glm::scale(m, scale);
	return m;
}

glm::mat4 Transform::blood_mat4()
{
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