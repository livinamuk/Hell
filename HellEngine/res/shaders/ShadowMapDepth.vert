#version 400 core
layout (location = 0) in vec3 aPos;
layout (location = 5) in ivec4 aBoneID;
layout (location = 6) in vec4 aBoneWeight;
layout (location = 7) in mat4 instanceMatrix;

uniform mat4 model;
uniform bool instanced;

uniform bool hasAnimation;
uniform mat4 skinningMats[64];

uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;



out mat4 modelOut;

void mai2n()
{

	
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

	// Animated
	if (hasAnimation)
	{
		for(int i=0;i<4;i++) 
		{
			mat4 jointTransform = skinningMats[int(aBoneID[i])];
			vec4 posePosition =  jointTransform  * vertexPosition * aBoneWeight[i];

			totalLocalPos += posePosition;		
		}
		worldPos = modelMatrix * totalLocalPos;
		gl_Position = projection * view * worldPos;
	}
	else // Not animated
	{
		worldPos = modelMatrix * vec4(aPos, 1.0);
		FragPos = (worldPos).xyz;
		gl_Position = projection * view * vec4(FragPos, 1);
	}



	//attrNormal = Normal;
	//attrTangent = aTangent;
	//attrBiTangent = aBitangent;
	//vec3 T = normalize(vec3(modelMatrix * vec4(aTangent,   0.0)));
	//vec3 B = normalize(vec3(modelMatrix * vec4(aBitangent, 0.0)));
	//vec3 N = normalize(vec3(modelMatrix * vec4(Normal,    0.0)));
	//TBN = mat3(T, B, N);
}


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