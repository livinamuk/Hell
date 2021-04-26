#version 420 core

layout (location = 0) out vec3 Composite;

layout (binding = 0) uniform sampler2D FinalLighting_Texture;
layout (binding = 1) uniform sampler2D blur0;
layout (binding = 2) uniform sampler2D blur1;
layout (binding = 3) uniform sampler2D blur2;
layout (binding = 4) uniform sampler2D blur3;
layout (binding = 5) uniform sampler2D GlassBlurTexture;
layout (binding = 6) uniform sampler2D GlassSurfaceTexture;
layout (binding = 7) uniform sampler2D ScreenSpaceReflectionsTexture;

in vec2 TexCoords;

//uniform sampler2D Emissive_Texture;
//uniform float contrast;

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

void main()
{
	vec3 lighting = vec3(texture(FinalLighting_Texture, TexCoords));

	// Glass Blur
	//vec3 glassBlur  = vec3(texture(GlassBlurTexture, TexCoords));
	//if (glassBlur != 0)
	//	lighting = glassBlur;
		
	// Glass Blur
	vec3 glassSurface  = vec3(texture(GlassSurfaceTexture, TexCoords));
	vec3 SSR = vec3(texture(ScreenSpaceReflectionsTexture, TexCoords));

	// Tone mapping
	//lighting = Tonemap_ACES(lighting);
    // Gamma compressionlighting
	//lighting = OECF_sRGBFast(lighting);

	vec3 blur = texture(blur0, TexCoords).rgb;
	blur += texture(blur1, TexCoords).rgb;
	blur += texture(blur2, TexCoords).rgb;
	blur += texture(blur3, TexCoords).rgb;
	
	
	vec3 final = (lighting) + (glassSurface );

		// Tone mapping
	final = Tonemap_ACES(final);
    // Gamma compressionlighting
	final = OECF_sRGBFast(final);
	
	Composite = final + blur  ;
	//Composite = SSR;
}