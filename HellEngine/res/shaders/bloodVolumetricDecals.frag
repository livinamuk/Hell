#version 420 core
#extension GL_ARB_shader_texture_lod : require

layout (location = 0) out vec4 gAlbedo;
layout (location = 1) out vec3 gRMA;

layout (binding = 0) uniform sampler2D depthTexture;
layout (binding = 1) uniform sampler2D normalTexture;
layout (binding = 2) uniform sampler2D DecalTex4;
layout (binding = 3) uniform sampler2D DecalTex6;
layout (binding = 4) uniform sampler2D DecalTex7;
layout (binding = 5) uniform sampler2D DecalTex9;


 uniform float  u_Time;
 uniform int  u_Type;


uniform mat4 inverseProjectionMatrix;
uniform mat4 inverseViewMatrix;
uniform mat4 model;
uniform mat4 inverseModel;
uniform float screenWidth;
uniform float screenHeight;
uniform vec3 targetPlaneSurfaceNormal;
uniform int writeRoughnessMetallic;

//in vec3 Normal;
in mat3 TBN;
in mat4 inverseModelMatrix;

uniform vec3 _DecalForwardDirection;

float saturate(float value)
{
	return clamp(value, 0.0, 1.0);
}

void main()
{    
    // Get the screen coordinates
    vec2 screenCoords = gl_FragCoord.xy / vec2(screenWidth, screenHeight);

    // Discard if the pixel is in the blockout map 
    float blockout = texture(normalTexture, screenCoords).a;
    if (blockout > 0.5)
        discard;

    // world normal
    vec3 worldNormal = texture(normalTexture, screenCoords).rgb;
    float angleToFloor = abs(dot(worldNormal, vec3(0, 1, 0)));
    float angle = dot(worldNormal, _DecalForwardDirection);  
    if(abs(angle) < 0.125 && angleToFloor < 0.5) 
        discard;

    // Backstab test. If world normal is facing away from original bullet angle.
    if (angle < -0.5)
        discard;

    // Calculate the fragment's world position
    float z = texture(depthTexture, vec2(screenCoords)).x * 2.0f - 1.0f;
    vec4 clipSpacePosition = vec4(screenCoords * 2.0 - 1.0, z, 1.0);
    vec4 viewSpacePosition = inverseProjectionMatrix * clipSpacePosition;
    viewSpacePosition /= viewSpacePosition.w;
    vec4 worldSpacePosition = inverseViewMatrix * viewSpacePosition;
    vec3 WorldPos = worldSpacePosition.xyz;

    // Don't draw on ceiling
	if (WorldPos.y > 2.39)
		discard;

	vec4 objectPosition = inverse(model) * vec4(WorldPos, 1.0);
    vec3 stepVal = (vec3(0.5, 0.5, 0.5) - abs(objectPosition.xyz)) * 1000;
    stepVal.x = saturate(stepVal.x);
    stepVal.y = saturate(stepVal.y);
    stepVal.z = saturate(stepVal.z);
    float projClipFade = stepVal.x * stepVal.y * stepVal.z;
	// Add 0.5 to get texture coordinates.
	vec2 decalTexCoord = vec2(objectPosition.x, objectPosition.z) + 0.5;

     vec4 mask = vec4(0.0);
    if (u_Type == 4)
        mask = texture(DecalTex4, decalTexCoord);
    if (u_Type == 6)
        mask = texture(DecalTex6, decalTexCoord);
    if (u_Type == 7)
        mask = texture(DecalTex7, decalTexCoord);
    if (u_Type == 9)
        mask = texture(DecalTex9, decalTexCoord);   
        
    vec4 res = vec4(0); 
    res.a = saturate(mask.a * 2);
    res.a *= projClipFade;

    if (mask.a * 2 * projClipFade < 0.1) 
       discard;

    vec3 _TintColor = vec3(0.32, 0, 0);
    float alphaMask = (mask.a  * 20) * res.a;
    alphaMask = clamp(alphaMask, 0, 1);
    float colorMask = (mask.a * 5) * res.a;
    colorMask = clamp(colorMask , 0, 1);
    colorMask = mask.a * 0.5;
    res.a = mask.a * min(u_Time * 5, 1);
    res.rgb = mix(_TintColor.rgb, _TintColor.rgb * 0.2, mask.z * colorMask * 0.75);
    float magic = 0.67;

    gAlbedo.rgb = mix(vec3(magic), res.rgb * 1.45, res.a * projClipFade);
    gAlbedo *= 0.725;
   
   // Roughness / metallic / ambient
   // gAlbedo = vec4(0.125 , 0.25, 1, 0);
    gRMA = vec3(0.125 , 0.25, 1);
}
