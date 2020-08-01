#version 420 core

layout (binding = 0) uniform sampler2D u_PosTex;
layout (binding = 1) uniform sampler2D u_NormTex;

//uniform vec4 u_Color;
//uniform float u_LightIntensity;
//uniform vec3 u_SunPos;

in vec3 v_WorldNormal;
in vec3 v_ViewDir;

out vec4 FragColor;

float saturate(float value)
{
	return clamp(value, 0.0, 1.0);
}

void main() {

	vec3 worldNormal = normalize(v_WorldNormal);
	vec3 viewDirNormal = normalize(v_ViewDir);

	float fresnel = clamp(1 - dot(worldNormal, viewDirNormal), 0, 1);
	//float fresnel = saturate(
	//TODO gamma corection might be needed

	vec4 u_Color = vec4(0.25, 0, 0, 1);
	float u_LightIntensity = 1;
	vec3 u_SunPos = vec3(2, 2.2, -0.2); // light position


	vec3 grabColor = u_LightIntensity * 0.75 * vec3(1, 1, 1);
	grabColor = grabColor * u_Color.rgb;
	grabColor = mix(grabColor * 0.15, grabColor, fresnel);
	grabColor = min(grabColor, u_Color.rgb * 0.55);

	float light = max(0.001, dot(worldNormal, normalize(u_SunPos.xyz)));           
	light = pow(light, 50) * 10;

	vec3 color = grabColor + clamp(light, 0, 1) * u_LightIntensity * vec3(1, 1, 1);

	//color *= 0.5;

	FragColor = vec4(color, 1);
	//FragColor = vec4(fresnel, fresnel, fresnel, 1);
}