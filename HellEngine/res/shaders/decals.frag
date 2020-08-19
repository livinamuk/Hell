#version 420 core

layout (location = 0) out vec3 gAlbedo;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gRMA;

layout (binding = 0) uniform sampler2D depthTexture;
layout (binding = 1) uniform sampler2D normalTexture;
layout (binding = 2) uniform sampler2D diffuseTexture;

uniform mat4 inverseProjectionMatrix;
uniform mat4 inverseViewMatrix;
uniform mat4 model;
uniform float screenWidth;
uniform float screenHeight;
uniform vec3 targetPlaneSurfaceNormal;
uniform int writeRoughnessMetallic;

in vec3 Normal;
in mat3 TBN;

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

    if (abs(objectPosition.x) > 0.5)
        discard;
    if (abs(objectPosition.y) > 0.5)
        discard;

    vec3 normal = texture(normalTexture, vec2(depthCoords.s, depthCoords.t)).rgb;

	// Add 0.5 to get texture coordinates.
	vec2 decalTexCoord = objectPosition.xy + 0.5;
    vec4 color = texture(diffuseTexture, decalTexCoord);

	if (color.a < 0.9)
		discard;

    gAlbedo = color.rgb;	
    gNormal = targetPlaneSurfaceNormal;

    gRMA = vec3(1, 0, 1);
}