
///////////
// NOTES //
///////////

// You are raising the basecolor texture to the power of 2.2 like learnopengl
// but then you are tone mapping it differently.

// You are multiplying the indirect IBL by the light attenuation. 
// Otherwise the whole fucking room is bright.
// It's wrong but looks so much better.	

// You decided NOT to ditch the IBL specular contribution.
// It doesn't add too much to the scene PLUS introudces that lighting you hate at the end of the shotty.
// ALSO by ditching it you no longer have to render a cubemap at the player location which is a huuuuge saving.

#version 330 core

layout (location = 0) out vec4 FragColor;


uniform sampler2D ALB_Texture;
uniform sampler2D NRM_Texture;
uniform sampler2D RMA_Texture;
uniform sampler2D Depth_Texture;
uniform sampler2D Position_Texture;
uniform samplerCube ShadowMap;

//layout (binding = 7) uniform samplerCube ShadowMap;


// IBL
uniform samplerCube Env_Cubemap;
uniform sampler2D Env_LUT;
uniform sampler2D BRDF_LUT;

uniform vec3 lightPosition;
uniform float lightAttenuationConstant;
uniform float lightAttenuationLinear;
uniform float lightAttenuationExp;
uniform float lightStrength;
uniform vec3 lightColor;

const float EPS = 1e-4;
const float PI = 3.14159265359;

uniform float far_plane;

uniform mat4 inverseProjectionMatrix;
uniform mat4 inverseViewMatrix;
uniform mat4 viewMatrix;

uniform float screenWidth;
uniform float screenHeight;

uniform float room_lowerX;
uniform float room_lowerZ;
uniform float room_upperX;
uniform float room_upperZ;

// array of offset direction for sampling
vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float ShadowCalculation(vec3 fragPos, vec3 viewPos)
{
    vec3 fragToLight = fragPos - lightPosition;
    float currentDepth = length(fragToLight);

    float shadow = 0.0;
    float bias = 0.2;
	int samples = 20;

    float viewDistance = length(viewPos - fragPos);
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(ShadowMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= far_plane;
        if(currentDepth * currentDepth - bias > closestDepth * closestDepth)
        {    
			shadow += 1.0;
		}
    }
    shadow /= float(samples);
	
   // return shadow;

   vec2 res = vec2(1024, 1024);

   float visibility = 1.0;
  // float fragDepth = shadowCoord.z;
   float fragDepth = currentDepth;
	float nearestOccluderDepth = texture(ShadowMap, fragToLight).x;
	nearestOccluderDepth *= far_plane;

	if(fragDepth > 0){
		// float s = 50.0;
		float s = 1000.0;
		// 	// visibility -= 0.2*(1.0-texture( shadowMap, vec3((shadowCoord.xy + poissonDisk[index]/700.0)*res,  (shadowCoord.z-bias)/shadowCoord.w) ));
		visibility = clamp( exp( s * (nearestOccluderDepth - fragDepth)), 0.0, 1.0 );
	}

	//return 1 - visibility;
	
    diskRadius = (1.0 + (viewDistance / far_plane)) / 150.0;
	fragDepth = fragDepth;
	samples = 20;

	for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(ShadowMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= far_plane;
		closestDepth = closestDepth;
        
		 //if(currentDepth * currentDepth - bias > closestDepth * closestDepth)
		 if(fragDepth > 0)
		{
			float s = 150.0;
			float b = 0.0275;
			visibility += 1 - clamp( exp( s * (closestDepth - b - fragDepth)), 0.0, 1.0 );
		}
    }


	 visibility /= float(samples);
	return visibility;
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



//------------------------------------------------------------------------------
// Tone mapping and transfer functions
//------------------------------------------------------------------------------

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

float saturate(float value)
{
	return clamp(value, 0.0, 1.0);
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

    
float minMax(float d) {
    return max(abs(d),0.0001);
}

float sqr(float x) {
    return x * x;
}



float CalculateAttenuation(
    in vec3  surfNorm,
    in vec3  vector,
    in float k)
{
    float d = max(dot(surfNorm, vector), 0.0);
 	return (d / ((d * (1.0 - k)) + k));
}

/**
 * GGX/Schlick-Beckmann attenuation for analytical light sources.
 */
float CalculateAttenuationAnalytical(
    in vec3  surfNorm,
    in vec3  toLight,
    in vec3  toView,
    in float roughness)
{
    float k = pow((roughness + 1.0), 2.0) * 0.125;

    // G(l) and G(v)
    float lightAtten = CalculateAttenuation(surfNorm, toLight, k);
    float viewAtten  = CalculateAttenuation(surfNorm, toView, k);

    // Smith
    return (lightAtten * viewAtten);
}

/**
 * Calculates the Fresnel reflectivity.
 */
vec3 CalculateFresnel(
    in vec3 surfNorm,
    in vec3 toView,
    in vec3 fresnel0)
{
	float d = max(dot(surfNorm, toView), 0.0);
    float p = ((-5.55473 * d) - 6.98316) * d;

    // Fresnel-Schlick approximation
    return fresnel0 + ((1.0 - fresnel0) * pow(1.0 - d, 5.0));
    // modified by Spherical Gaussian approximation to replace the power, more efficient
    return fresnel0 + ((1.0 - fresnel0) * pow(2.0, p));
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

// ----------------------------------------------------------------------------
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}   

void main()
{
	 // Calculate texture co-ordinate
    vec2 gScreenSize = vec2(screenWidth, screenHeight);
    vec2 TexCoord = gl_FragCoord.xy / gScreenSize;
	vec2 TexCoords = TexCoord;

    // Get the Fragment Z position (from the depth buffer)
    float z = texture(Depth_Texture, vec2(TexCoord.s, TexCoord.t)).x * 2.0f - 1.0f;
    vec4 clipSpacePosition = vec4(vec2(TexCoord.s, TexCoord.t) * 2.0 - 1.0, z, 1.0);
    vec4 viewSpacePosition = inverseProjectionMatrix * clipSpacePosition;
    
	// Get the Fragment XYZ position (perspective division, via it's depth value)
    viewSpacePosition /= viewSpacePosition.w;
    vec4 worldSpacePosition = inverseViewMatrix * viewSpacePosition;
    vec3 WorldPos = worldSpacePosition.xyz;

	// Get GBuffer Data
	//vec3 albedo = texture(ALB_Texture, TexCoords).rgb
	vec3 albedo = pow(texture(ALB_Texture, TexCoords).rgb, vec3(2.2));
	float roughness = texture(RMA_Texture, TexCoords).r;	
	float metallic  = texture(RMA_Texture, TexCoords).g;
	float ao = texture(RMA_Texture, TexCoords).b;

	//metallic *= metallic;

    // Light data
	float attenuation = CaclulateAttenuation(WorldPos, lightPosition, lightAttenuationConstant) * lightStrength;
	vec3 radiance = lightColor * attenuation;

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

    vec3 viewPos = vec3(inverseViewMatrix * vec4(0, 0, 0, 1));

    vec3 n = normalize(texture(NRM_Texture, TexCoords).rgb);
	vec3 v = normalize(viewPos - WorldPos);    // Vector from surface point to camera
	
    vec3 l = normalize(lightPosition - WorldPos);   // Vector from surface point to light
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
	vec3 directLighting = NdotL * (diffuseContrib + specContrib) * radiance;

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
	const float MAX_REFLECTION_LOD = 16; // was 4 !!!!!!!
	vec3 R = reflect(-v, n); 
	vec3 prefilteredColor = textureLod(Env_Cubemap, R,  roughness * MAX_REFLECTION_LOD).rgb;
	prefilteredColor = vec3(lightColor * 0.005);
	vec2 brdf  = texture(BRDF_LUT, vec2(NdotV, roughness)).rg;
	vec3 f = fresnelSchlickRoughness(NdotV, f0, roughness);
    vec3 indirectSpecular = prefilteredColor * (f * brdf.x + brdf.y);

	vec3 kS = F;
	vec3 kD = 1.0 - kS;
	kD *= 1.0 - metallic;	
    //vec3 indirectLighting = (kD * indirectDiffuse + indirectSpecular) * ao;
	vec3 indirectLighting = (kD * indirectDiffuse) * ao; // no specular!!!!!!!!!!!!

	float shadowFactor = ShadowCalculation(WorldPos, viewPos);
    
	float shadow = min((1.0 - shadowFactor), NdotL);
	shadow =  1.0 - shadowFactor;


	// Make indirect diffuse somewhat affected by shadows
//	indirectLighting *= shadowFactor / 2.5;
//		indirectLighting *= (shadowFactor);// * 10.0);

//	shadow *= NdotL;
//	float bias = 0.05;
//	if (WorldPos.z > 2 + bias)
	//	shadow = min((1.0 - shadowFactor), NdotL * NdotL * NdotL);

	if ((WorldPos.x < room_lowerX) || (WorldPos.z < room_lowerZ)  || (WorldPos.x > room_upperX) || (WorldPos.z > room_upperZ))
		indirectLighting *= vec3(shadow);// * NdotL;

		//indirectLighting = vec3(0,0,0);
	//	directLighting = vec3(0,0,0);
	color = ((shadow * directLighting) + indirectLighting) * attenuation;
	//color = indirectLighting;
	//color = ((shadow * directLighting) + (indirectLighting * shadow)) * attenuation;
	
    FragColor = vec4(color, 1.0);
}