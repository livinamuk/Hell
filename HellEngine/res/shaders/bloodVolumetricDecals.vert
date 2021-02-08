#version 330 core

layout(location = 0) in vec3 aPos;

uniform mat4 pv;
uniform mat4 model;

out vec3 FragPos;

void main()
{
	// Instancing?
	mat4 modelMatrix = model;

	vec4 worldPos = modelMatrix * vec4(aPos, 1.0);
	FragPos = worldPos.xyz;
	gl_Position = pv * worldPos;
}


