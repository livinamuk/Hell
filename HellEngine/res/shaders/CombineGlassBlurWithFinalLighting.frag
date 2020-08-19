#version 420 core

layout (location = 0) out vec4 FragOut;

layout (binding = 0) uniform sampler2D GlassBlurTexture;

in vec2 TexCoords;

void main()
{
	vec3 glassBlur = vec3(texture(GlassBlurTexture, TexCoords));
	if (glassBlur == 0)
		discard;

	FragOut = vec4(glassBlur, 1);
}