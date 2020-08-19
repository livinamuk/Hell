#version 330 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec2 a_Texcoord;
layout (location = 2) in vec3 a_Normal;

uniform mat4 u_MatrixProjection;
uniform mat4 u_MatrixView;
uniform mat4 u_MatrixWorld;

out vec3 Normal;
out vec2 Texcoord;

void main() {
	Texcoord = a_Texcoord;
	//Normal = a_Normal;
	gl_Position = u_MatrixProjection * u_MatrixView * u_MatrixWorld * vec4(a_Position.xyz, 1.0);

	// this is fucking slow. do it on CPU.
	mat3 normalMatrix = transpose(inverse(mat3(u_MatrixWorld))); 
	Normal= normalize(normalMatrix * a_Normal);
}