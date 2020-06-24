#version 330 core

uniform int       u_FrameIndex;
uniform int       u_CountRaw;
uniform int       u_CountColumn;
uniform float     u_TimeLerp;
uniform sampler2D u_MainTexture;
uniform bool u_isBlood;

in  vec2 v_Texcoord;
out vec4 FragColor;


vec3 Tonemap_ACES(const vec3 x) {
    // Narkowicz 2015, "ACES Filmic Tone Mapping Curve"
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return (x * (a * x + b)) / (x * (c * x + d) + e);
}

vec3 OECF_sRGBFast(const vec3 linear) {
    return pow(linear, vec3(1.0 / 2.2));
}


void main() {

	vec2 sizeTile =  vec2(1.0 / u_CountColumn, 1.0 / u_CountRaw);

	int frameIndex0 = u_FrameIndex;
	int frameIndex1 = u_FrameIndex + 1;


	vec2 tileOffset0 = ivec2(frameIndex0 % u_CountColumn, frameIndex0 / u_CountColumn) * sizeTile;
	vec2 tileOffset1 = ivec2(frameIndex1 % u_CountColumn, frameIndex1 / u_CountColumn) * sizeTile;


	vec4 color0 = texture(u_MainTexture, tileOffset0 + v_Texcoord * sizeTile);
	vec4 color1 = texture(u_MainTexture, tileOffset1 + v_Texcoord * sizeTile);
	
	
	vec3 tint = vec3(1, 1, 1);

	if (u_isBlood)
		tint = vec3(0.5,0,0);
	
	vec4 color = mix(color0, color1, u_TimeLerp) * vec4(tint, 1.0);
	
	if (u_isBlood)
	{
		if (color.a < 0.1)
			discard;
	}
	else
	{
		if (frameIndex0 > 1)
			color.a *= 0.12;
	}	


	//if (color.a < 0.1)
	//	discard;
	
//	color = vec4(1,0,0,1);		

// Tone mapping
//	color.rgb = Tonemap_ACES(color.rgb);
    // Gamma compressionlighting
	//color.rgb = OECF_sRGBFast(color.rgb);

	FragColor = color;


}