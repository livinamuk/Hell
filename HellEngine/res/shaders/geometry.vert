#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBinormal;
layout (location = 5) in ivec4 aBoneID;
layout (location = 6) in vec4 aBoneWeight;


uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;

out vec3 FragPos;
out vec2 TexCoord;
out vec3 Normal;
out mat3 TBN;
out vec4 test;

uniform bool hasAnimation;
uniform mat4 skinningMats[64];

void main()
{
	TexCoord = aTexCoord;
	mat4 modelMatrix = model;

	vec4 worldPos;
	vec4 totalLocalPos = vec4(0.0);
	vec4 totalNormal = vec4(0.0);
    mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));

	
	// Animated
	if (hasAnimation) {
		for(int i=0;i<4;i++) {
			mat4 jointTransform = skinningMats[int(aBoneID[i])];
			vec4 posePosition = jointTransform * vec4(aPos, 1.0);
			vec4 worldNormal = jointTransform * vec4(aNormal, 0.0);
			totalLocalPos += posePosition * aBoneWeight[i];		
			totalNormal += worldNormal * aBoneWeight[i];
		}
		worldPos = modelMatrix * totalLocalPos;
		Normal = totalNormal.xyz;
		//test = vec4(aBlendIndex[0])	;
	}

	//	if (hasAnimation) {
	//	for(int i=0;i<4;i++) {
	//		mat4 jointTransform = skinningMats[aBoneID[i]];
	//		vec4 posePosition = jointTransform * vec4(aPos, 1.0);
	//		vec4 worldNormal = jointTransform * vec4(aNormal, 0.0);
	//		totalLocalPos += posePosition * aBoneWeight[i];		
	//		totalNormal += worldNormal * aBoneWeight[i];
	//	}
	//	worldPos = modelMatrix * totalLocalPos;
	//	Normal = totalNormal.xyz;
	//}

	// Not animated
	if (!hasAnimation) {
		worldPos = modelMatrix * vec4(aPos, 1.0);
		Normal = aNormal;
	}

	vec3 T = normalize(vec3(model * vec4(aTangent,   0.0)));
	vec3 B = normalize(vec3(model * vec4(aBinormal, 0.0)));
	vec3 N = normalize(vec3(model * vec4(Normal,    0.0)));
	TBN = mat3(T, B, N);

	FragPos = (worldPos).xyz;

	gl_Position = projection * view * vec4(FragPos, 1);
}