#version 330 core
out vec4 FragColor;
in vec3 WorldPos;

uniform samplerCube environmentMap;

void main()
{		
    //vec3 envColor = texture(environmentMap, WorldPos).rgb;
    
    // HDR tonemap and gamma correct
    //envColor = envColor / (envColor + vec3(1.0));
    //envColor = pow(envColor, vec3(1.0/2.2)); 
    //FragColor = vec4(envColor, 1.0);

	
// FragColor = vec4(vec3(textureLod(environmentMap, WorldPos, 2).rgb), 1);
 FragColor = vec4(vec3(textureLod(environmentMap, WorldPos, 0.5).rgb), 1);
    
	//FragColor = vec4(1, 0, 0, 1);
}
