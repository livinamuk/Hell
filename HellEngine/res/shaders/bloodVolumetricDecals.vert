#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in mat4 instanceMatrix;

uniform mat4 pv;
uniform mat4 model;
uniform bool instanced;

out vec3 FragPos;
out mat4 inverseModelMatrix;

void main()
{
	// Instancing?	
	mat4 modelMatrix;
	if (!instanced)
		modelMatrix = model;
	else 
		modelMatrix = instanceMatrix;

		//inverseModelMatrix = (instanceMatrix);
		//modelMatrix = modelMatrix;

	vec4 worldPos = modelMatrix * vec4(aPos, 1.0);
	FragPos = worldPos.xyz;
	gl_Position = pv * worldPos;
}


