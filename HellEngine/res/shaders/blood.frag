#version 330 core

uniform int       u_FrameIndex;
uniform int       u_CountRaw;
uniform int       u_CountColumn;
uniform float     u_TimeLerp;
uniform sampler2D u_MainTexture;

in  vec2 v_Texcoord;
out vec4 FragColor;

void main() {

	vec2 sizeTile =  vec2(1.0 / u_CountColumn, 1.0 / u_CountRaw);

	int frameIndex0 = u_FrameIndex;
	int frameIndex1 = u_FrameIndex + 1;


	vec2 tileOffset0 = ivec2(frameIndex0 % u_CountColumn, frameIndex0 / u_CountColumn) * sizeTile;
	vec2 tileOffset1 = ivec2(frameIndex1 % u_CountColumn, frameIndex1 / u_CountColumn) * sizeTile;


	vec4 color0 = texture(u_MainTexture, tileOffset0 + v_Texcoord * sizeTile);
	vec4 color1 = texture(u_MainTexture, tileOffset1 + v_Texcoord * sizeTile);
	
	vec3 tint = vec3(1, 0, 0);
	vec4 color = mix(color0, color1, u_TimeLerp) * vec4(tint, 1.0);
	

	if (color.a < 0.1)
		discard;
	
	//color = vec4(1,0,0,1);
	FragColor = color;
}