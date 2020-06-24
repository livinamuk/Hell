#version 400 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBinormal;
layout (location = 5) in vec4 aBlendIndex;
layout (location = 6) in vec4 aBlendWeight;

//uniform mat4 view;
uniform mat4 model;
//uniform mat4 projection;

out vec2 gTexCoord;
out vec3 gNormal;

uniform bool hasAnimation;
uniform mat4 skinningMats[64];

void main()
{
	gTexCoord = aTexCoord;
	mat4 modelMatrix = model;

	vec4 worldPos;
	vec4 totalLocalPos = vec4(0.0);
	vec4 totalNormal = vec4(0.0);
    mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));

	
	// Animated
	if (hasAnimation) {
		for(int i=0;i<4;i++) {
			mat4 jointTransform = skinningMats[int(aBlendIndex[i])];
			vec4 posePosition = jointTransform * vec4(aPos, 1.0);
			vec4 worldNormal = jointTransform * vec4(aNormal, 0.0);
			totalLocalPos += posePosition * aBlendWeight[i];		
			totalNormal += worldNormal * aBlendWeight[i];
		}
		worldPos = modelMatrix * totalLocalPos;
		gNormal = totalNormal.xyz;
	}

	// Not animated
	if (!hasAnimation) {
		worldPos = modelMatrix * vec4(aPos, 1.0);
		gNormal = aNormal;
	}

	gNormal = mat3(model) * gNormal;   

	vec3 FragPos = worldPos.xyz;
	//gl_Position = projection * view * vec4(FragPos, 1);
	gl_Position = vec4(FragPos, 1);
}