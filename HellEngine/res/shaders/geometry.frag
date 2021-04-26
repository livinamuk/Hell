#version 420 core

layout (location = 0) out vec4 gAlbedo;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec3 gRMA;
layout (location = 3) out vec4 gEmissive;

layout (binding = 0) uniform sampler2D ALB_Texture0;
layout (binding = 1) uniform sampler2D NRM_Texture0;
layout (binding = 2) uniform sampler2D RMA_Texture0;
layout (binding = 3) uniform sampler2D ALB_Texture1;
layout (binding = 4) uniform sampler2D NRM_Texture1;
layout (binding = 5) uniform sampler2D RMA_Texture1;
layout (binding = 6) uniform sampler2D EMMISIVE_Texture;
layout (binding = 7) uniform sampler2D DECAL_MAP_Texture;

uniform bool useRoughnessMetallicUniforms;
uniform float roughnessUniform;
uniform float metallicUniform;

uniform bool blockoutDecals;
uniform bool u_hasDecalMap;

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

in vec3 attrNormal;
in vec3 attrTangent;
in vec3 attrBiTangent;

uniform mat4 model;

void main()
{
	//////////////////////////////////////
	// Recalculate texture co-ordinates //
 
	vec2 finalTexCoords = TexCoord;

	if (TEXTURE_FLAG == 3)								
		finalTexCoords = vec2(FragPos.x, FragPos.z) * 0.4;		// Floor Regular
	else if (TEXTURE_FLAG == 4)								
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
	
//	ALB = vec4(0.5, 0.5, 0.5, 1);
//	NRM = vec4(0, 0, 1, 0);

	////////////////////
	// GBuffer Output //
	////////////////////
	
	vec3 normal_map = NRM.xyz * 2.0 - 1.0;
	//vec3 bitangent = cross(attrNormal, attrTangent);

//	vec3 T = normalize(vec3(model * vec4(attrTangent,   0.0)));
//	vec3 B = normalize(vec3(model * vec4(attrBiTangent, 0.0)));
	//vec3 N = normalize(vec3(model * vec4(attrNormal,    0.0)));
	//mat3 TBNN = mat3(T, B, N);

//	mat3 tbn = mat3(normalize(attrTangent), normalize(attrBiTangent), normalize(attrNormal));
	//mat3 tbn = mat3(attrTangent, attrBiTangent, attrNormal); // emit aNormal from vs to fs (do skinning if you have to)
//	vec3 normal = normalize(tbn * normal_map);
	

	//mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));

	//normal  = normalize(model * vec4(normal, 0.0)).xyz;
	//normal  = normalize((model * vec4(normal, 0.0)).xyz);

	//normal = normalize(modelOut * vec4(normal, 0.0)).xyz;
//	gNormal = normal ;//* 0.5 + 0.5;

	mat3 tbn = mat3(normalize(attrTangent), normalize(attrBiTangent), normalize(attrNormal));

	vec3 normal = normalize(tbn * (texture(NRM_Texture0, finalTexCoords).rgb * 2.0 - 1.0));
	gNormal.rgb = normal ;//* 0.5 + 0.5;

	

	gAlbedo = ALB + vec4(ColorAdd, 0);
	gRMA.rgb = RMA.rgb;
	
	if (useRoughnessMetallicUniforms) {
		gRMA.r = roughnessUniform;
		gRMA.g = metallicUniform;
		gRMA.b = 1;
	}

	// Has emissive map
	if (hasEmissive) {
		gEmissive.rgb = texture(EMMISIVE_Texture, finalTexCoords).r * emissiveColor;
	}
	else
		gEmissive.rgb = vec3(0, 0, 0);


	// SSR mask
	if (TEXTURE_FLAG == 3 || TEXTURE_FLAG == 4)	// if true receivesSSR
		gEmissive.a = 0;
	else
		gEmissive.a = 1;

	//if (normal.y < -0.95)
	//	gEmissive.a = 1;



	//	gAlbedo.rgb = vec3(gAlbedo.a);
	//gAlbedo.a = 1;
	//gAlbedo.a = 1;
	
	//gAlbedo = vec4(1) ;//* MaterialID;
	if (round(MaterialID) == 1 ) {
	//	gAlbedo =  texture(ALB_Texture1, finalTexCoords);
	//	gRMA = vec3(1, 0, 0);
	}

//	gBlood = vec4(0, 1, 0, 1);
	//gAlbedo = vec4(0.5);
	
	if (blockoutDecals)
		gNormal.a = 1;
	else
		gNormal.a = 0;

	//if (blockoutDecals)
	//	gNormal.rgb = vec3(1);
	//	else
	//	gNormal.rgb = vec3(0);

		//gAlbedo.rgb = vec3(blockoutDecals);  

		
		vec4 decalMapColor = texture(DECAL_MAP_Texture, finalTexCoords);

		vec4 color = vec4(vec3(decalMapColor), 1);

		// DECAL MAP
		if (u_hasDecalMap)
			gAlbedo = gAlbedo;// * color;
		
}
