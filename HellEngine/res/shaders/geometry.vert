#version 420 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;
layout (location = 5) in ivec4 aBoneID;
layout (location = 6) in vec4 aBoneWeight;
layout (location = 7) in int aMaterialID;
layout (location = 8) in mat4 instanceMatrix;

uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;

out vec3 FragPos;
out vec2 TexCoord;
out vec3 Normal;
out mat3 TBN;
out float MaterialID;

out vec3 attrNormal;
out vec3 attrTangent;
out vec3 attrBiTangent;

uniform bool hasAnimation;
uniform mat4 skinningMats[64];
uniform bool instanced;


out mat4 modelOut;

void main()
{
	MaterialID = aMaterialID;
	TexCoord = aTexCoord;	

	
	mat4 modelMatrix;
	if (!instanced)
		modelMatrix = model;
		else 
		modelMatrix = instanceMatrix;

	vec4 worldPos;
	vec4 totalLocalPos = vec4(0.0);
	vec4 totalNormal = vec4(0.0);
   // mat3 normalMatrix = transpose(inverse(mat3(modelMatrix))); // do this on the cpu. its fucking slow.
	
	vec4 vertexPosition =  vec4(aPos, 1.0);
	vec4 vertexNormal = vec4(aNormal, 0.0);

	// Animated
	if (hasAnimation)
	{
		for(int i=0;i<4;i++) 
		{
			mat4 jointTransform = skinningMats[int(aBoneID[i])];
			vec4 posePosition =  jointTransform  * vertexPosition * aBoneWeight[i];
			vec4 worldNormal = jointTransform * vertexNormal * aBoneWeight[i];

			totalLocalPos += posePosition;		
			totalNormal += worldNormal;
		}
		worldPos = modelMatrix * totalLocalPos;
		Normal = totalNormal.xyz;
		gl_Position = projection * view * worldPos;
	}
	else // Not animated
	{
		worldPos = modelMatrix * vec4(aPos, 1.0);
		Normal = aNormal;
		FragPos = (worldPos).xyz;
		gl_Position = projection * view * vec4(FragPos, 1);
	}


	attrNormal = (model * vec4(Normal, 0.0)).xyz;
	attrTangent = (model * vec4(aTangent, 0.0)).xyz;
	attrBiTangent = (model * vec4(aBitangent, 0.0)).xyz;

	//attrNormal = Normal;
	//attrTangent = aTangent;
	//attrBiTangent = aBitangent;
	//vec3 T = normalize(vec3(modelMatrix * vec4(aTangent,   0.0)));
	//vec3 B = normalize(vec3(modelMatrix * vec4(aBitangent, 0.0)));
	//vec3 N = normalize(vec3(modelMatrix * vec4(Normal,    0.0)));
	//TBN = mat3(T, B, N);
}