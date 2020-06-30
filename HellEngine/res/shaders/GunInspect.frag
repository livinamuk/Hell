#version 420 core

out vec4 FragColor;

layout (binding = 0) uniform sampler2D ALB_Texture;
layout (binding = 1) uniform sampler2D NRM_Texture;
layout (binding = 2) uniform sampler2D RMA_Texture;

layout (binding = 5) uniform sampler2D Env_LUT;

in vec2 TexCoords;
in vec4 FragPos;
in vec3 Normal;

const float PI = 3.14159265359;

const vec3 viewPos = vec3(0,0,0);
const vec3 lightPosition = vec3(0,5,-5);
const vec3 lightColor = vec3(1,1,1);
const float lightRadius = 20;
const float lightStrength = 155;
const float lightMagic = 15;


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
    float magic = lightMagic;
    float dist = length(worldPos - lightPosition);
    float radius = lightRadius;
    float num = saturate(1 - (dist / radius) * (dist / radius) * (dist / radius) * (dist / radius));
    return num * num / (dist * dist + magic);
}

vec3 specularReflection(PBRInfo pbrInputs)
{
	return pbrInputs.reflectance0 + (pbrInputs.reflectance90 - pbrInputs.reflectance0) * pow(clamp(1.0 - pbrInputs.VdotH, 0.0, 1.0), 5.0);
}

float geometricOcclusion(PBRInfo pbrInputs)
{
	float NdotL = pbrInputs.NdotL;
	float NdotV = pbrInputs.NdotV;
	float r = pbrInputs.alphaRoughness;

	float attenuationL = 2.0 * NdotL / (NdotL + sqrt(r * r + (1.0 - r * r) * (NdotL * NdotL)));
	float attenuationV = 2.0 * NdotV / (NdotV + sqrt(r * r + (1.0 - r * r) * (NdotV * NdotV)));
	return attenuationL * attenuationV;
}

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

vec3 Tonemap_ACES(const vec3 x) {
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


/***********************************************************************/
// Spherical Harmonics Lib

// Constants, see here: http://en.wikipedia.org/wiki/Table_of_spherical_harmonics
#define k01 0.2820947918 // sqrt(  1/PI)/2
#define k02 0.4886025119 // sqrt(  3/PI)/2
#define k03 1.0925484306 // sqrt( 15/PI)/2
#define k04 0.3153915652 // sqrt(  5/PI)/4
#define k05 0.5462742153 // sqrt( 15/PI)/4
#define k06 0.5900435860 // sqrt( 70/PI)/8
#define k07 2.8906114210 // sqrt(105/PI)/2
#define k08 0.4570214810 // sqrt( 42/PI)/8
#define k09 0.3731763300 // sqrt(  7/PI)/4
#define k10 1.4453057110 // sqrt(105/PI)/4

// Y_l_m(s), where l is the band and m the range in [-l..l]
float SphericalHarmonic( in int l, in int m, in vec3 n )
{
	//----------------------------------------------------------
	if( l==0 )          return   k01;

	//----------------------------------------------------------
	if( l==1 && m==-1 ) return  -k02*n.y;
	if( l==1 && m== 0 ) return   k02*n.z;
	if( l==1 && m== 1 ) return  -k02*n.x;

	//----------------------------------------------------------
	if( l==2 && m==-2 ) return   k03*n.x*n.y;
	if( l==2 && m==-1 ) return  -k03*n.y*n.z;
	if( l==2 && m== 0 ) return   k04*(3.0*n.z*n.z-1.0);
	if( l==2 && m== 1 ) return  -k03*n.x*n.z;
	if( l==2 && m== 2 ) return   k05*(n.x*n.x-n.y*n.y);
	//----------------------------------------------------------

	return 0.0;
}

mat3 shEvaluate(vec3 n) {
	mat3 r;
	r[0][0] =  SphericalHarmonic(0,  0, n);
	r[0][1] = -SphericalHarmonic(1, -1, n);
	r[0][2] =  SphericalHarmonic(1,  0, n);
	r[1][0] = -SphericalHarmonic(1,  1, n);

	r[1][1] =  SphericalHarmonic(2, -2, n);
	r[1][2] = -SphericalHarmonic(2, -1, n);
	r[2][0] =  SphericalHarmonic(2,  0, n);
	r[2][1] = -SphericalHarmonic(2,  1, n);
	r[2][2] =  SphericalHarmonic(2,  2, n);
	return r;
}

// Recovers the value of a SH function in the direction dir.
float shUnproject(mat3 functionSh, vec3 dir)
{
	mat3 sh = shEvaluate(dir);
	return
		dot(functionSh[0], sh[0]) +
		dot(functionSh[1], sh[1]) +
		dot(functionSh[2], sh[2]);
}

const vec3 convCoeff = vec3(1.0, 2.0/3.0, 1.0/4.0);
mat3 shDiffuseConvolution(mat3 sh) {
	mat3 r = sh;

	r[0][0] *= convCoeff.x;

	r[0][1] *= convCoeff.y;
	r[0][2] *= convCoeff.y;
	r[1][0] *= convCoeff.y;

	r[1][1] *= convCoeff.z;
	r[1][2] *= convCoeff.z;
	r[2][0] *= convCoeff.z;
	r[2][1] *= convCoeff.z;
	r[2][2] *= convCoeff.z;

	return r;
}

vec3 shToColor(mat3 shR, mat3 shG, mat3 shB, vec3 rayDir) {
	vec3 rgbColor = vec3(
		shUnproject(shR, rayDir),
		shUnproject(shG, rayDir),
		shUnproject(shB, rayDir));

	// A "max" is usually recomended to avoid negative values (can happen with SH)
	rgbColor = max(vec3(0.0), vec3(rgbColor));
	return rgbColor;
}

void main()
{


	float roughness = texture(RMA_Texture, TexCoords).r + 0.001; 
	float metallic = texture(RMA_Texture, TexCoords).g; 
	vec3 albedo = pow(texture(ALB_Texture, TexCoords).rgb, vec3(2.2));

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



	vec3 radiance = CaclulateAttenuation(FragPos.xyz, lightPosition, lightRadius) * lightColor * lightStrength;
	color = NdotL * (diffuseContrib + specContrib) * radiance;




	// Spherical Harmonics
	mat3 shR, shG, shB;
    #define SH_FILL(x, y) \
    { \
        vec3 samp = texelFetch(Env_LUT, ivec2(x, y), 0).rgb; \
        shR[x][y] = samp.r; \
        shG[x][y] = samp.g; \
        shB[x][y] = samp.b; \
    }
    SH_FILL(0, 0)
    SH_FILL(0, 1)
    SH_FILL(0, 2)
    SH_FILL(1, 0)
    SH_FILL(1, 1)
    SH_FILL(1, 2)
    SH_FILL(2, 0)
    SH_FILL(2, 1)
    SH_FILL(2, 2)
    #undef SH_FILL		
	mat3 shRD = shDiffuseConvolution(shR);
    mat3 shGD = shDiffuseConvolution(shG);
    mat3 shBD = shDiffuseConvolution(shB);

	// Indirect diffuse
	vec3 irradiance = shToColor(shRD, shGD, shBD, n); 
	vec3 indirectDiffuse = irradiance * diffuseColor;
    
	// Indirect specular
////	vec3 R = reflect(-v, n); 
//	vec3 prefilteredColor = textureLod(Env_Cubemap, R,  roughness * MAX_REFLECTION_LOD).rgb;
//	prefilteredColor = vec3(lightColor * 0.005);
//	vec2 brdf  = texture(BRDF_LUT, vec2(NdotV, roughness)).rg;
//	vec3 f = fresnelSchlickRoughness(NdotV, f0, roughness);
 //   vec3 indirectSpecular = prefilteredColor * (f * brdf.x + brdf.y);

	vec3 kS = F;
	vec3 kD = 1.0 - kS;
	kD *= 1.0 - metallic;	
   vec3 indirectLighting = (kD * indirectDiffuse);


	color += indirectLighting;
	
	FragColor = vec4(vec3(color), 1);
	FragColor = vec4(vec3(color), 1);

		// Tone mapping
	FragColor.xyz = Tonemap_ACES(FragColor.xyz);
    // Gamma compressionlighting
	FragColor.xyz = OECF_sRGBFast(FragColor.xyz);
}