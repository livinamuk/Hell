#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBinormal;
layout (location = 5) in vec4 aBlendIndex;
layout (location = 6) in vec4 aBlendWeight;

uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;
out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;
uniform bool hasAnimation;
uniform mat4 skinningMats[64];

void main()
{
	vec3 localPos = aPos;
	TexCoord = aTexCoord;
	Normal = aNormal;
	if( hasAnimation )
	{
		localPos = vec3( 0 );
		for(int i = 0; i < 4; ++i)
		{
			int matIdx = int( aBlendIndex[i] );
			float weight = aBlendWeight[i];
			mat4 blendMat = skinningMats[matIdx];
			localPos += (blendMat * vec4(aPos, 1.0f) * weight).xyz;
		}
	}
	
	vec4 worldPos = model * vec4(localPos, 1.0);
	gl_Position = projection * view * worldPos;

	FragPos = worldPos.xyz;
}