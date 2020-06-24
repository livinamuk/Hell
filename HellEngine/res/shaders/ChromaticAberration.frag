#version 420 core

layout (location = 0) out vec3 FragOut;
layout (binding = 0) uniform sampler2D InputTexture;

in vec2 TexCoords;
uniform float time;

#define HASHSCALE1 443.8975

//  1 out, 3 in...
float hash13(vec3 p3)
{
	p3  = fract(p3 * HASHSCALE1);
    p3 += dot(p3, p3.yzx + 19.19);
    return fract((p3.x + p3.y) * p3.z);
}

vec3 ApplyGrain( vec2 vUV, vec3 col, float amount )
{
    float h = hash13( vec3(vUV, time) );    
    col *= (h * 2.0 - 1.0) * amount + (1.0f -amount);
    return col;
}

void main()
{
	vec3 inputTexture = vec3(texture(InputTexture, TexCoords));
	FragOut = vec3(inputTexture.r, 0, 0);
	
	 vec2 destCoord = TexCoords;
     int sampleCount = 50;
    float blur = 0.25; 
    float falloff = 05.0; 

    vec2 direction = normalize(destCoord - 0.5); 
    vec2 velocity = direction * blur * pow(length(destCoord - 0.5), falloff);
	float inverseSampleCount = 1.0 / float(sampleCount); 
    
    mat3x2 increments = mat3x2(velocity * 1.0 * inverseSampleCount,
                               velocity * 2.0 * inverseSampleCount,
                               velocity * 4.0 * inverseSampleCount);

    vec3 accumulator = vec3(0);
    mat3x2 offsets = mat3x2(0); 
    
    for (int i = 0; i < sampleCount; i++) {
        accumulator.r += texture(InputTexture, destCoord + offsets[0]).r; 
        accumulator.g += texture(InputTexture, destCoord + offsets[1]).g; 
        accumulator.b += texture(InputTexture, destCoord + offsets[2]).b; 
        
        offsets -= increments;
    }
	vec3 finalColor = vec3(accumulator / float(sampleCount));

    // Film grain
    float amount = 0.05;
	finalColor = ApplyGrain(destCoord, finalColor, amount); 

	// OUTPUT
	FragOut = vec3(finalColor);
}