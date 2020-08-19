#version 420 core

layout (location = 0) out vec4 fragout;

layout (binding = 0) uniform sampler2D gBufferFinalLighting_Texture;
layout (binding = 1) uniform sampler2D NRM_Texture0;
layout (binding = 2) uniform sampler2D RMA_Texture0;
layout (binding = 3) uniform sampler2D ALB_Texture1;
layout (binding = 4) uniform sampler2D NRM_Texture1;
layout (binding = 5) uniform sampler2D RMA_Texture1;

in vec3 Normal;
in vec3 WorldPos;
in vec2 TexCoord;
in mat3 TBN;

uniform mat4 inverseViewMatrix;
uniform mat4 viewMatrix;
uniform mat4 model;

uniform float screenWidth;
uniform float screenHeight;

vec4 blur5(sampler2D image, vec2 uv, vec2 resolution, vec2 direction) {
  vec4 color = vec4(0.0);
  vec2 off1 = vec2(1.3333333333333333) * direction;
  color += texture2D(image, uv) * 0.29411764705882354;
  color += texture2D(image, uv + (off1 / resolution)) * 0.35294117647058826;
  color += texture2D(image, uv - (off1 / resolution)) * 0.35294117647058826;
  return color; 
}

void main()
{   
	vec2 TexCoords = gl_FragCoord.xy / vec2(screenWidth, screenHeight);
	vec3 gBufferFinalLighting = texture(gBufferFinalLighting_Texture, TexCoords).rgb;
	
	// vec2 uv = vec2(gl_FragCoord.xy / vec2.xy);
    bool flip = false;

      if (flip) {
        TexCoords.y = 1.0 - TexCoords.y;
      }

      vec2 direction = vec2(1.25);
  vec4 blur = blur5(gBufferFinalLighting_Texture, TexCoords,  vec2(screenWidth, screenHeight), direction);

  	blur.r -= 0.0035;
	blur.b += 0.0035;
//	fragout.g += 0.0025;

	//vec3 color = gBufferFinalLighting;
	//color.r = 1;
	fragout = vec4(blur.xyz, 1);
}