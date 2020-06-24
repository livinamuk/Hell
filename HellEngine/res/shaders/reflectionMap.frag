#version 400 core

layout (location = 0) out vec4 FragColor;

uniform bool useRoughnessMetallicUniforms;
uniform float roughnessUniform;
uniform float metallicUniform;

in vec2 TexCoord;
in vec4 FragPos;
in vec3 Normal;

uniform vec3 emissiveColor;
uniform bool hasEmissiveMap;
uniform int TEXTURE_FLAG;
uniform vec2 TEXTURE_SCALE;

//uniform mat4 inverseViewMatrix;
uniform vec3 viewPos;

uniform int i;

uniform vec3 lightPosition;
uniform float lightAttenuationConstant;
uniform float lightAttenuationLinear;
uniform float lightAttenuationExp;
uniform float lightStrength;
uniform vec3 lightColor;

uniform vec2 texOffset;

uniform sampler2D ALB_Texture;
uniform sampler2D NRM_Texture;
uniform sampler2D RMA_Texture;

const float PI = 3.14159265359;
const vec3 noColor = vec3(0.0, 0.0, 0.0);

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


float saturate(float value)
{
	return clamp(value, 0.0, 1.0);
}

float CaclulateAttenuation(vec3 worldPos, vec3 lightPosition, float lightRadius)
{
    float magic = lightAttenuationExp; // was 1
    float dist = length(worldPos - lightPosition);
    float radius = lightAttenuationConstant;
    float num = saturate(1 - (dist / radius) * (dist / radius) * (dist / radius) * (dist / radius));
    return num * num / (dist * dist + magic);
}

// The following equation models the Fresnel reflectance term of the spec equation (aka F())
// Implementation of fresnel from [4], Equation 15
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
	// Recalculate texture co-ordinates
	vec2 finalTexCoords = TexCoord;
	// Floor
	if (TEXTURE_FLAG == 3)								
		finalTexCoords = vec2(FragPos.x, FragPos.z) * 0.4;		// Regular
	if (TEXTURE_FLAG == 4)								
		finalTexCoords = vec2(FragPos.z, FragPos.x) * 0.4;		// Rotate 90 degrees

	float roughness = texture(RMA_Texture, finalTexCoords).r + 0.001; 
	float metallic = texture(RMA_Texture, finalTexCoords).g; 
	//vec3 albedo = texture(ALB_Texture, finalTexCoords).rgb;
	vec3 albedo = pow(texture(ALB_Texture, finalTexCoords).rgb, vec3(2.2));

	if (useRoughnessMetallicUniforms) {
		roughness = roughnessUniform;
		metallic= metallicUniform;
	}
	
	/////////
	// PBR //
	/////////
	
	vec3 color = vec3(0);
    vec3 f0 = vec3(0.04);
    vec3 diffuseColor = albedo * (vec3(1.0) - f0);
	diffuseColor *= 1.0 - metallic;
		
	float alphaRoughness = roughness * roughness;
	vec3 specularColor = mix(f0, albedo, metallic);

	// Compute reflectance.
	float reflectance = max(max(specularColor.r, specularColor.g), specularColor.b);

	// For typical incident reflectance range (between 4% to 100%) set the grazing reflectance to 100% for typical fresnel effect.
	// For very low reflectance range on highly diffuse objects (below 4%), incrementally reduce grazing reflecance to 0%.
	float reflectance90 = clamp(reflectance * 25.0, 0.0, 1.0);
	vec3 specularEnvironmentR0 = specularColor.rgb;
	vec3 specularEnvironmentR90 = vec3(1.0, 1.0, 1.0) * reflectance90;

    //vec3 viewPos = vec3(inverseViewMatrix * vec4(0, 0, 0, 1));

    vec3 n = Normal;//normalize(texture(NRM_Texture, TexCoords).rgb);
	vec3 v = normalize(viewPos - FragPos.xyz);    // Vector from surface point to camera
	
    vec3 l = normalize(lightPosition - FragPos.xyz);   // Vector from surface point to light
	vec3 h = normalize(v + l);    // Half vector between both l and v            
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
	vec3 radiance = CaclulateAttenuation(FragPos.xyz, lightPosition, lightAttenuationConstant) * lightColor * lightStrength;
	color = NdotL * (diffuseContrib + specContrib) * radiance;

	FragColor = vec4(vec3(color), 1);
}