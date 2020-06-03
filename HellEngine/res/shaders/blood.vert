#version 330 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec2 a_Texcoord;

uniform mat4 u_MatrixProjection;
uniform mat4 u_MatrixView;
uniform mat4 u_MatrixWorld;


out vec2 v_Texcoord;

void main() {
	v_Texcoord = a_Texcoord;
	gl_Position = u_MatrixProjection * u_MatrixView * u_MatrixWorld * vec4(a_Position.xyz, 1.0);
}