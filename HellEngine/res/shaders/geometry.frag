#version 420 core

layout (location = 0) out vec4 gAlbedo;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gRMA;
layout (location = 3) out vec3 gbuffer4;

layout (binding = 0) uniform sampler2D ALB_Texture0;
layout (binding = 1) uniform sampler2D NRM_Texture0;
layout (binding = 2) uniform sampler2D RMA_Texture0;
layout (binding = 3) uniform sampler2D ALB_Texture1;
layout (binding = 4) uniform sampler2D NRM_Texture1;
layout (binding = 5) uniform sampler2D RMA_Texture1;
layout (binding = 6) uniform sampler2D EMMISIVE_Texture;

uniform bool useRoughnessMetallicUniforms;
uniform float roughnessUniform;
uniform float metallicUniform;

in vec2 TexCoord;
in vec3 FragPos;
in mat3 TBN;
in vec3 Normal;
in float MaterialID;

uniform vec3 ColorAdd;

uniform bool hasAnimation;
in vec4 test;

uniform bool hasEmissive;
uniform vec3 emissiveColor;

//uniform bool hasEmissiveMap;
uniform int TEXTURE_FLAG;
uniform vec2 TEXTURE_SCALE;

uniform vec2 texOffset;


void main()
{
	//////////////////////////////////////
	// Recalculate texture co-ordinates //
 
	vec2 finalTexCoords = TexCoord;

	if (TEXTURE_FLAG == 3)								
		finalTexCoords = vec2(FragPos.x, FragPos.z) * 0.4;		// Floor Regular
	if (TEXTURE_FLAG == 4)								
		finalTexCoords = vec2(FragPos.z, FragPos.x) * 0.4;		// Flooor Rotated 90 degrees
	
	//////////////////////
	// Read Texture Set //
	//////////////////////

	vec4 ALB;
	vec4 RMA;
	vec4 NRM;

	if (round(MaterialID) == 1) {
		ALB =  texture(ALB_Texture1, finalTexCoords);
		RMA =  texture(RMA_Texture1, finalTexCoords);
		NRM =  texture(NRM_Texture1, finalTexCoords);
	}
	else {
		ALB =  texture(ALB_Texture0, finalTexCoords);
		RMA =  texture(RMA_Texture0, finalTexCoords);
		NRM =  texture(NRM_Texture0, finalTexCoords);
	}
	
	////////////////////
	// GBuffer Output //
	////////////////////
	
	gNormal = NRM.rgb;
	gNormal = normalize(gNormal.rgb * 2.0 - 1.0);
	gNormal = normalize(TBN * gNormal.rgb);

	gAlbedo = ALB + vec4(ColorAdd, 0);
	gRMA = RMA.rgb;
	
	if (useRoughnessMetallicUniforms) {
		gRMA.r = roughnessUniform;
		gRMA.g = metallicUniform;
		gRMA.b = 1;
	}

	// Has emissive map
	if (hasEmissive) {
		gbuffer4 = texture(EMMISIVE_Texture, finalTexCoords).r * emissiveColor;
	}
	else
		gbuffer4 = vec3(0, 0, 0);

	//	gAlbedo.rgb = vec3(gAlbedo.a);
	//gAlbedo.a = 1;
	//gAlbedo.a = 1;
	
	//gAlbedo = vec4(1) ;//* MaterialID;
	if (round(MaterialID) == 1 ) {
	//	gAlbedo =  texture(ALB_Texture1, finalTexCoords);
	//	gRMA = vec3(1, 0, 0);
	}
	
	//	gAlbedo =  RMA;//texture(RMA_Texture1, finalTexCoords);
	//gAlbedo.rgb = vec3(round(MaterialID));

	//gAlbedo.rgb = vec3(gRMA.g);//



	if (hasAnimation) {
	//	gAlbedo = vec3(test.x);
	}
}
