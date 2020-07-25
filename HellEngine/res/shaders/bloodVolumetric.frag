#version 330 core

uniform vec4 u_Color;
uniform float u_LightIntensity;
uniform vec3 u_SunPos;

in vec3 v_WorldNormal;
in vec3 v_ViewDir;

out vec4 FragColor;

void main() {

	vec3 worldNormal = normalize(v_WorldNormal);
	vec3 viewDirNormal = normalize(v_ViewDir);

	float fresnel = clamp(1 - dot(worldNormal, viewDirNormal), 0, 1);

	//TODO gamma corection might be needed

	vec3 grabColor = u_LightIntensity * 0.75 * vec3(1, 1, 1);
	grabColor = grabColor * u_Color.rgb;
	grabColor = mix(grabColor * 0.15, grabColor, fresnel);
	grabColor = min(grabColor, u_Color.rgb * 0.55);

	float light = max(0.001, dot(worldNormal, normalize(u_SunPos.xyz)));           
	light = pow(light, 50) * 10;

	vec3 color = grabColor + clamp(light, 0, 1) * u_LightIntensity * vec3(1, 1, 1);

	FragColor = vec4(color, 1);
	//FragColor = vec4(0, 0, 1, 1);
}