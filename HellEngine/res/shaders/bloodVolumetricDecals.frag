#version 420 core

layout (location = 0) out vec4 gAlbedo;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gRMA;

layout (binding = 0) uniform sampler2D depthTexture;
layout (binding = 1) uniform sampler2D normalTexture;
layout (binding = 2) uniform sampler2D Decal_norm;
layout (binding = 3) uniform sampler2D Decal_mask;



uniform mat4 inverseProjectionMatrix;
uniform mat4 inverseViewMatrix;
uniform mat4 model;
uniform float screenWidth;
uniform float screenHeight;
uniform vec3 targetPlaneSurfaceNormal;
uniform int writeRoughnessMetallic;

in vec3 Normal;
in mat3 TBN;

float saturate(float value)
{
	return clamp(value, 0.0, 1.0);
}

void main()
{    
    // Get the Fragment Z position (from the depth buffer)
    vec2 depthCoords = gl_FragCoord.xy / vec2(screenWidth, screenHeight);
    float z = texture(depthTexture, vec2(depthCoords.s, depthCoords.t)).x * 2.0f - 1.0f;
    vec4 clipSpacePosition = vec4(vec2(depthCoords.s, depthCoords.t) * 2.0 - 1.0, z, 1.0);
    vec4 viewSpacePosition = inverseProjectionMatrix * clipSpacePosition;

    // Get the Fragment XYZ position (perspective division, via it's depth value)
    viewSpacePosition /= viewSpacePosition.w;
    vec4 worldSpacePosition = inverseViewMatrix * viewSpacePosition;
    vec3 WorldPos = worldSpacePosition.xyz;
	
	vec4 objectPosition = inverse(model) * vec4(WorldPos, 1.0);

  ////  if (abs(objectPosition.x) > 0.5)
  //      discard;
  //  if (abs(objectPosition.y) > 0.5)
  //      discard;



  //  vec3 normal = texture(normalTexture, vec2(depthCoords.s, depthCoords.t)).rgb;

	// Add 0.5 to get texture coordinates.
	vec2 decalTexCoord = vec2(objectPosition.x, objectPosition.z) + 0.5;
    vec4 color = texture(Decal_mask, decalTexCoord);

    
    vec4 normal =  texture(Decal_norm, vec2(decalTexCoord.s, decalTexCoord.t));
    vec3 mask =  texture(Decal_mask, vec2(decalTexCoord.s, decalTexCoord.t)).rgb;
  
    vec4 res = vec4(0); 
    res.a = saturate(normal.a * 2);
    if (res.a < 0.01) 
       discard; //a little optimization

    normal.xy = normal.xy* 2 - 1;
    normal.xyz = normalize(vec3(normal.x, 1, normal.y));

    float _Cutout = 0;
    vec3 _SunPos = vec3(2, 2.2, -0.2); // light position
    float intensity = 1;
    vec3 _TintColor = vec3(0.32, 0, 0);

    float cutout = 0.5 + _Cutout * 0.5; //_cutout it's public variable
    float alphaMask = ((mask.r - (cutout * 2 - 1)) * 20) * res.a;
    alphaMask = clamp(alphaMask, 0, 1);
    float colorMask = ((mask.r - (cutout * 2 - 1)) * 5) * res.a;
    colorMask = clamp(colorMask , 0, 1);

    res.a = alphaMask;
    float light = max(0.001, dot(normal.xyz, normalize(_SunPos.xyz)));
    light = pow(light, 150) * 3 * intensity; //intensity [0-1]
    light *= (1 - mask.z * colorMask); //in some big blood decals I use different channels for alpha/thin

    res.rgb = mix(_TintColor.rgb, _TintColor.rgb * 0.2, mask.z * colorMask * 0.75) + light;
    gAlbedo = res.rgba * 0.5 * (1 - res.a) + res.rgba * res.a;
 
    gAlbedo = vec4(res.rgb * res.a, 1);
    gNormal = normal.xyz;

    gRMA = vec3(1, 0, 1);
}