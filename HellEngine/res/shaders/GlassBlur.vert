#version 440 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBinormal;

uniform mat4 model;
uniform mat4 viewMatrix;
uniform mat4 projection;

out vec3 WorldPos;
out vec2 TexCoord;
out vec3 Normal;
out mat3 TBN;

void main()
{
    TexCoord = aTexCoord;	
    WorldPos = vec4(model * vec4(aPosition, 1.0)).xyz;
    gl_Position = projection * viewMatrix * model * vec4(aPosition, 1.0);	
	
	vec3 T = normalize(vec3(model * vec4(aTangent,   0.0)));
	vec3 B = normalize(vec3(model * vec4(aBinormal, 0.0)));
	vec3 N = normalize(vec3(model * vec4(aNormal,    0.0)));
	TBN = mat3(T, B, N);



	Normal = aBinormal;
}