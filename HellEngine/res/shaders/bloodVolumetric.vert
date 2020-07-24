#version 330 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec2 a_Texcoord;

uniform mat4 u_MatrixProjection;
uniform mat4 u_MatrixView;
uniform mat4 u_MatrixWorld;
uniform mat4 u_MatrixInverseWorld;

uniform sampler2D u_PosTex;
uniform sampler2D u_NormTex;
uniform float u_BoundingMax;
uniform float u_BoundingMin;
uniform float u_Speed;
uniform int u_NumOfFrames;
uniform vec4 u_HeightOffset;
uniform float u_TimeInFrames;//TODO this should be particular to each instance
uniform vec3 u_WorldSpaceCameraPos;

out vec3 v_WorldNormal;
out vec3 v_ViewDir;

void main() {

	float currentSpeed = u_Speed / u_NumOfFrames;

	vec2 coord = vec2(a_Texcoord.x, (u_TimeInFrames + a_Texcoord.y));
	vec4 texturePos = textureLod(u_PosTex, coord, 0);
	vec4 textureNorm = textureLod(u_NormTex, coord, 0);

	//TODO gamma corection might be needed

	float expand = u_BoundingMax - u_BoundingMin;
	texturePos.xyz *= expand;
	texturePos.xyz += u_BoundingMin;
	texturePos.x *= -1;

	vec3 vertexPos = texturePos.xyz + u_HeightOffset.xyz;
	
	v_WorldNormal = textureNorm.xyz * 2; 
	v_WorldNormal.x = 1 - v_WorldNormal.x;

	v_ViewDir = (u_MatrixInverseWorld * vec4(u_WorldSpaceCameraPos, 1.0)).xyz - vertexPos;

	gl_Position = u_MatrixProjection * u_MatrixView * u_MatrixWorld * vec4(vertexPos, 1.0);

	//gl_Position = u_MatrixProjection * u_MatrixView * u_MatrixWorld * vec4(a_Position, 1.0);
}