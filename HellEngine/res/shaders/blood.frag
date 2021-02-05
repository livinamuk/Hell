#version 330 core

out vec4 FragColor;

uniform int       u_FrameIndex;
uniform int       u_CountRaw;
uniform int       u_CountColumn;
uniform float     u_TimeLerp;
uniform sampler2D u_MainTexture;
uniform bool u_isBlood;

in vec2 Texcoord;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 u_ViewPos;

uniform vec3 lightPosition[20];
uniform float lightRadius[20];
uniform float lightMagic[20];
uniform float lightStrength[20];
uniform vec3 lightColor[20];

const float EPS = 1e-4;
const float PI = 3.14159265359;

// PBR SHIT
float saturate(float value) {
	return clamp(value, 0.0, 1.0);
}
float CaclulateAttenuation(vec3 worldPos, vec3 lightPosition, float radius, float magic) {
    float dist = length(worldPos - lightPosition);
    float num = saturate(1 - (dist / radius) * (dist / radius) * (dist / radius) * (dist / radius));
    return num * num / (dist * dist + magic);
}
struct PBRInfo {
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
vec3 specularReflection(PBRInfo pbrInputs) {
	return pbrInputs.reflectance0 + (pbrInputs.reflectance90 - pbrInputs.reflectance0) * pow(clamp(1.0 - pbrInputs.VdotH, 0.0, 1.0), 5.0);
}
float geometricOcclusion(PBRInfo pbrInputs) {
	float NdotL = pbrInputs.NdotL;
	float NdotV = pbrInputs.NdotV;
	float r = pbrInputs.alphaRoughness;
	float attenuationL = 2.0 * NdotL / (NdotL + sqrt(r * r + (1.0 - r * r) * (NdotL * NdotL)));
	float attenuationV = 2.0 * NdotV / (NdotV + sqrt(r * r + (1.0 - r * r) * (NdotV * NdotV)));
	return attenuationL * attenuationV;
}
float microfacetDistribution(PBRInfo pbrInputs) {
	float roughnessSq = pbrInputs.alphaRoughness * pbrInputs.alphaRoughness;
	float f = (pbrInputs.NdotH * roughnessSq - pbrInputs.NdotH) * pbrInputs.NdotH + 1.0;
	return roughnessSq / (PI * f * f);
}
vec3 diffuse(PBRInfo pbrInputs) {
	return pbrInputs.diffuseColor / PI;
}
// END PBR SHIT





void main() {

	vec2 sizeTile =  vec2(1.0 / u_CountColumn, 1.0 / u_CountRaw);

	int frameIndex0 = u_FrameIndex;
	int frameIndex1 = u_FrameIndex + 1;


	vec2 tileOffset0 = ivec2(frameIndex0 % u_CountColumn, frameIndex0 / u_CountColumn) * sizeTile;
	vec2 tileOffset1 = ivec2(frameIndex1 % u_CountColumn, frameIndex1 / u_CountColumn) * sizeTile;


	vec4 color0 = texture(u_MainTexture, tileOffset0 + Texcoord * sizeTile);
	vec4 color1 = texture(u_MainTexture, tileOffset1 + Texcoord * sizeTile);
	
	
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




	/////////
	// PBR //
	/////////
	
	if (!u_isBlood)
	{
		FragColor = vec4(color);
	}
	else
	{
		vec3 diffuseColor = vec3(color.r, 0, 0);	
		float roughness = 1;
		float metallic = 1;

		vec3 f0 = vec3(0.04);
		float alphaRoughness = roughness * roughness;
		vec3 specularColor = mix(f0, diffuseColor, metallic);

		// Compute reflectance.
		float reflectance = max(max(specularColor.r, specularColor.g), specularColor.b);
		float reflectance90 = clamp(reflectance * 25.0, 0.0, 1.0);
		vec3 specularEnvironmentR0 = specularColor.rgb;
		vec3 specularEnvironmentR90 = vec3(1.0, 1.0, 1.0) * reflectance90;

		//vec3 viewPos = vec3(inverseViewMatrix * vec4(0, 0, 0, 1));
	
	//	vec3 n = Normal;	
		vec3 v = normalize(u_ViewPos - FragPos); 
		vec3 n = vec3(-v);

		vec3 l;
		int light_count = 6;

		vec3 finalColor = vec3(0);

		for (int i=0; i < light_count; i++)	
		{	
			float attenuation = CaclulateAttenuation(FragPos, lightPosition[i], lightRadius[i], lightMagic[i]) * lightStrength[i];
			vec3 radiance = lightColor[i] * attenuation;
			l = normalize(lightPosition[i] - FragPos);

			// flip to face the light if required
			//if (dot(n, l) < 0)
			//	n *= -1;
	
			float NdotL = clamp(dot(n, l), 0.001, 1.0);
						
			vec3 h = normalize(v + l);            
			vec3 reflection = -normalize(reflect(v, n));
			reflection.y *= -1.0f;

			float NdotV = clamp(abs(dot(n, v)), 0.001, 1.0);
			float NdotH = clamp(dot(n, h), 0.0, 1.0);
			float LdotH = clamp(dot(l, h), 0.0, 1.0);
			float VdotH = clamp(dot(v, h), 0.0, 1.0);

			PBRInfo pbrInputs = PBRInfo(NdotL, NdotV, NdotH, LdotH, VdotH, roughness, metallic,	specularEnvironmentR0, specularEnvironmentR90, alphaRoughness, diffuseColor, specularColor);

			vec3 F = specularReflection(pbrInputs);
			float G = geometricOcclusion(pbrInputs);
			float D = microfacetDistribution(pbrInputs);

			vec3 diffuseContrib = (1.0 - F) * diffuse(pbrInputs);
			vec3 specContrib = F * G * D / (4.0 * NdotL * NdotV);
			vec3 directLighting = NdotL * (diffuseContrib + specContrib) ;
	
			finalColor += (directLighting) * radiance;
		}


	   FragColor = vec4(finalColor.rgb, 1);//* 0.35);
	  }

	//if (color.a < 0.1)
	//	discard;
	
//	color = vec4(1,0,0,1);		

// Tone mapping
//	color.rgb = Tonemap_ACES(color.rgb);
    // Gamma compressionlighting
	//color.rgb = OECF_sRGBFast(color.rgb);

	//FragColor = color;

	if (u_isBlood)
	{
		//FragColor = vec4(Normal, 1);

	//	FragColor = vec4(1, 0, 1 , 1);

		}
	
}