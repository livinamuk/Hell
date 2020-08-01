#version 400 core
layout (location = 0) in vec3 aPos;
layout (location = 5) in ivec4 aBoneID;
layout (location = 6) in vec4 aBoneWeight;
layout (location = 7) in mat4 instanceMatrix;

uniform mat4 model;
uniform bool instanced;

uniform bool hasAnimation;
uniform mat4 skinningMats[64];

void main()
{
	vec4 worldPos;
	vec4 totalLocalPos = vec4(0.0);

	mat4 modelMatrix;
	if (!instanced)
		modelMatrix = model;
	else 
		modelMatrix = instanceMatrix;

	// Animated
	if (hasAnimation) {
		for(int i=0;i<4;i++) {
			mat4 jointTransform = skinningMats[int(aBoneID[i])];
			vec4 posePosition = jointTransform * vec4(aPos, 1.0);
			totalLocalPos += posePosition * aBoneWeight[i];		
		}
		gl_Position = modelMatrix * totalLocalPos;
	}
	else
		gl_Position = modelMatrix * vec4(aPos, 1.0);
}