#version 420 core

layout (location = 0) out vec4 gAlbedo;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gRMA;

layout (binding = 0) uniform sampler2D u_PosTex;
layout (binding = 1) uniform sampler2D u_NormTex;

//uniform vec4 u_Color;
//uniform float u_LightIntensity;
//uniform vec3 u_SunPos;

in vec3 v_WorldNormal;
in vec3 v_ViewDir;

//out vec4 FragColor;

uniform mat4 u_MatrixWorld;

float saturate(float value)
{
	return clamp(value, 0.0, 1.0);
}

void main() {



 mat3 normalMatrix = transpose(inverse(mat3(u_MatrixWorld)));
    vec3 normal = normalize(normalMatrix * v_WorldNormal);

//	vec3 worldNormal = normalize(v_WorldNormal) * 2 - 1;
//	vec3 worldNormal = normalize(v_WorldNormal);
	vec3 worldNormal = v_WorldNormal * 2 - 1;

	normal = worldNormal;

vec3 camPos = v_ViewDir.xyz;
	vec3 viewDir = normalize(worldNormal - camPos);

	float fresnel = clamp(1 - dot(worldNormal, viewDir), 0, 1);
	//float fresnel = saturate(
	//TODO gamma corection might be needed

	vec4 u_Color = vec4(0.125, 0, 0, 1);
	float u_LightIntensity = 1;
	vec3 u_SunPos = vec3(2, 2.2, -0.2); // light position


	vec3 grabColor = u_LightIntensity * 0.75 * vec3(1, 1, 1);
	grabColor = vec3(0.3, 0, 0);
	grabColor = mix(grabColor * 0.3, grabColor, 1);
	//grabColor = min(grabColor, u_Color.rgb * 0.45);

	float light = max(0.001, dot(worldNormal, normalize(u_SunPos.xyz)));           
	light = pow(light, 4) * 20;

	vec3 color = grabColor + u_LightIntensity * light * vec3(1, 0.85, 0.85);

 


 gAlbedo.rgba = vec4(grabColor * 1, 1);


    gNormal = normal;//worldNormal.xyz;

    gRMA = vec3(0.15, 0.85, 0);
}