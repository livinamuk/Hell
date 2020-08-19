#version 420 core

layout (location = 0) out vec4 fragout;

layout (binding = 0) uniform sampler2D ALB_Texture0;
//layout (binding = 1) uniform sampler2D NRM_Texture0;
layout (binding = 2) uniform sampler2D RMA_Texture0;
layout (binding = 3) uniform sampler2D ALB_Texture1;
//layout (binding = 4) uniform sampler2D NRM_Texture1;
layout (binding = 5) uniform sampler2D RMA_Texture1;

layout (binding = 6) uniform sampler2D Dust_Texture0;
layout (binding = 7) uniform sampler2D Dust_Texture1;


in vec3 Normal;
in vec3 WorldPos;
in vec2 TexCoord;

uniform mat4 inverseViewMatrix;
uniform mat4 viewMatrix;
uniform mat4 model;

const float EPS = 1e-4;
const float PI = 3.14159265359;

uniform vec3 lightPosition[20];
uniform float lightRadius[20];
uniform float lightMagic[20];
uniform float lightStrength[20];
uniform vec3 lightColor[20];

float saturate(float value)
{
	return clamp(value, 0.0, 1.0);
}

float CaclulateAttenuation(vec3 worldPos, vec3 lightPosition, float radius, float magic)
{
    float dist = length(worldPos - lightPosition);
    float num = saturate(1 - (dist / radius) * (dist / radius) * (dist / radius) * (dist / radius));
    return num * num / (dist * dist + magic);
}

struct PBRInfo
{
	float NdotL;                  // cos angle between normal and light direction
	float NdotV;                  // cos angle between normal and view direction
	float NdotH;                  // cos angle between normal and half vector
	float LdotH;                  // cos angle between light direction and half vector
	float VdotH;                  // cos angle between view direction and half vector
	float perceptualRoughness;    // roughness value, as authored by the model creator (input to shader)
	float metalness;              // metallic value at the surface
	vec3 reflectance0;            // full reflectance color (normal incidence angle)
	vec3 reflectance90;           // reflectance color at grazing angle
	float alphaRoughness;         // roughness mapped to a more linear change in the roughness (proposed by [2])
	vec3 diffuseColor;            // color contribution from diffuse lighting
	vec3 specularColor;           // color contribution from specular lighting
};

vec3 specularReflection(PBRInfo pbrInputs)
{
	return pbrInputs.reflectance0 + (pbrInputs.reflectance90 - pbrInputs.reflectance0) * pow(clamp(1.0 - pbrInputs.VdotH, 0.0, 1.0), 5.0);
}

// This calculates the specular geometric attenuation (aka G()),
// where rougher material will reflect less light back to the viewer.
// This implementation is based on [1] Equation 4, and we adopt their modifications to
// alphaRoughness as input as originally proposed in [2].
float geometricOcclusion(PBRInfo pbrInputs)
{
	float NdotL = pbrInputs.NdotL;
	float NdotV = pbrInputs.NdotV;
	float r = pbrInputs.alphaRoughness;

	float attenuationL = 2.0 * NdotL / (NdotL + sqrt(r * r + (1.0 - r * r) * (NdotL * NdotL)));
	float attenuationV = 2.0 * NdotV / (NdotV + sqrt(r * r + (1.0 - r * r) * (NdotV * NdotV)));
	return attenuationL * attenuationV;
}

// The following equation(s) model the distribution of microfacet normals across the area being drawn (aka D())
// Implementation from "Average Irregularity Representation of a Roughened Surface for Ray Reflection" by T. S. Trowbridge, and K. P. Reitz
// Follows the distribution function recommended in the SIGGRAPH 2013 course notes from EPIC Games [1], Equation 3.
float microfacetDistribution(PBRInfo pbrInputs)
{
	float roughnessSq = pbrInputs.alphaRoughness * pbrInputs.alphaRoughness;
	float f = (pbrInputs.NdotH * roughnessSq - pbrInputs.NdotH) * pbrInputs.NdotH + 1.0;
	return roughnessSq / (PI * f * f);
}

vec3 diffuse(PBRInfo pbrInputs)
{
	return pbrInputs.diffuseColor / PI;
}


void main()
{   
	vec3 albedo = texture(ALB_Texture1, TexCoord).rgb;
	float roughness = texture(RMA_Texture1, TexCoord).r;	
	float metallic  = texture(RMA_Texture1, TexCoord).g;
	float ao = texture(RMA_Texture1, TexCoord).b;
	
	vec3 dust = vec3(texture(Dust_Texture0, TexCoord * 5).r) * 0.5;
	float dust2 = 1 - texture(Dust_Texture1, TexCoord * 5).r * 0.5;
	roughness += dust.r;
	metallic += dust2.r;

    // Light data
//	float attenuation = CaclulateAttenuation(WorldPos, lightPosition, lightAttenuationConstant, lightAttenuationExp) * lightStrength;
//	vec3 radiance = lightColor * attenuation;

	/////////
	// PBR //
	/////////
	
	//vec3 color = vec3(0);
    vec3 f0 = vec3(0.04);
    vec3 diffuseColor = albedo * (vec3(1.0) - f0);
	diffuseColor *= 1.0 - metallic;

	float alphaRoughness = roughness * roughness;
	vec3 specularColor = mix(f0, albedo, metallic);

	// Compute reflectance.
	float reflectance = max(max(specularColor.r, specularColor.g), specularColor.b);
	float reflectance90 = clamp(reflectance * 25.0, 0.0, 1.0);
	vec3 specularEnvironmentR0 = specularColor.rgb;
	vec3 specularEnvironmentR90 = vec3(1.0, 1.0, 1.0) * reflectance90;

    vec3 viewPos = vec3(inverseViewMatrix * vec4(0, 0, 0, 1));
	
    vec3 n = Normal;
	vec3 v = normalize(viewPos - WorldPos);    // Vector from surface point to camera
	

	vec3 l;
	int light_count = 6;

	vec3 color = vec3(0);

	for (int i=0; i < light_count; i++)	
	{
	
		float attenuation = CaclulateAttenuation(WorldPos, lightPosition[i], lightRadius[i], lightMagic[i]) * lightStrength[i];
		vec3 radiance = lightColor[i] * attenuation;
		l = normalize(lightPosition[i] - WorldPos);
		
	
		
		vec3 h = normalize(v + l);            
		vec3 reflection = -normalize(reflect(v, n));
		reflection.y *= -1.0f;

		float NdotL = clamp(dot(n, l), 0.001, 1.0);
		float NdotV = clamp(abs(dot(n, v)), 0.001, 1.0);
		float NdotH = clamp(dot(n, h), 0.0, 1.0);
		float LdotH = clamp(dot(l, h), 0.0, 1.0);
		float VdotH = clamp(dot(v, h), 0.0, 1.0);

		PBRInfo pbrInputs = PBRInfo(
			NdotL,
			NdotV,
			NdotH,
			LdotH,
			VdotH,
			roughness,
			metallic,
			specularEnvironmentR0,
			specularEnvironmentR90,
			alphaRoughness,
			diffuseColor,
			specularColor
		);

		vec3 F = specularReflection(pbrInputs);
		float G = geometricOcclusion(pbrInputs);
		float D = microfacetDistribution(pbrInputs);

		// Calculation of analytical lighting contribution
		vec3 diffuseContrib = (1.0 - F) * diffuse(pbrInputs);
		vec3 specContrib = F * G * D / (4.0 * NdotL * NdotV);
		// Obtain final intensity as reflectance (BRDF) scaled by the energy of the light (cosine law)
		vec3 directLighting = NdotL * (diffuseContrib + specContrib) * radiance;
	
		fragout = vec4(directLighting.xyz, 1);
		
		vec3 DUST0 = vec3(texture(Dust_Texture0, TexCoord * 5).r) * 0.025 * radiance;
		vec3 DUST1 = texture(Dust_Texture0, TexCoord).rgb;

		color += (directLighting + DUST0) * radiance;
	}
		
	fragout.rgb = color;
	//fragout.rgb = n;
}