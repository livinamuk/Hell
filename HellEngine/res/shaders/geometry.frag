#version 330 core

layout (location = 0) out vec3 gAlbedo;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gRMA;
layout (location = 3) out vec3 gbuffer4;

uniform bool useRoughnessMetallicUniforms;
uniform float roughnessUniform;
uniform float metallicUniform;

in vec2 TexCoord;
in vec3 FragPos;
in mat3 TBN;
in vec3 Normal;


uniform bool hasAnimation;
in vec4 test;

uniform bool hasEmissive;
uniform vec3 emissiveColor;

//uniform bool hasEmissiveMap;
uniform int TEXTURE_FLAG;
uniform vec2 TEXTURE_SCALE;

uniform vec2 texOffset;

uniform sampler2D ALB_Texture;
uniform sampler2D NRM_Texture;
uniform sampler2D RMA_Texture;

void main()
{
	//////////////////////////////////////
	// Recalculate texture co-ordinates //
 
	vec2 finalTexCoords = TexCoord;

	if (TEXTURE_FLAG == 3)								
		finalTexCoords = vec2(FragPos.x, FragPos.z) * 0.4;		// Floor Regular
	if (TEXTURE_FLAG == 4)								
		finalTexCoords = vec2(FragPos.z, FragPos.x) * 0.4;		// Flooor Rotated 90 degrees
	
	////////////////////
	// GBuffer Output //
	////////////////////

	gNormal = texture(NRM_Texture, finalTexCoords).rgb;
	gNormal = normalize(gNormal.rgb * 2.0 - 1.0);
	gNormal = normalize(TBN * gNormal.rgb);

	gAlbedo = texture(ALB_Texture, finalTexCoords).rgb;
	gRMA = texture(RMA_Texture, finalTexCoords).rgb;
	
	if (useRoughnessMetallicUniforms) {
		gRMA.r = roughnessUniform;
		gRMA.g = metallicUniform;
		gRMA.b = 1;
	}

	// Has emissive map
	if (hasEmissive) {
		gbuffer4 = vec3(texture(RMA_Texture, finalTexCoords).b) * emissiveColor;
	}
	else
		gbuffer4 = vec3(0, 0, 0);

		
		

	if (hasAnimation) {
	//	gAlbedo = vec3(test.x);
	}
}
