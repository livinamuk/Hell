#version 420 core

layout (location = 0) out vec3 fragColor;

layout (binding = 0) uniform sampler2D PREVIOUS_FRAME_TEXTURE;
layout (binding = 1) uniform sampler2D NORMAL_TEXTURE;
layout (binding = 2) uniform sampler2D DEPTH_TEXTURE;
layout (binding = 3) uniform sampler2D RMA_TEXTURE;
layout (binding = 4) uniform sampler2D EMISSIVE_TEXTURE;

uniform float screenWidth;
uniform float screenHeight;

uniform mat4 inverseProjectionMatrix;
uniform mat4 inverseViewMatrix;
uniform mat4 viewMatrix;

//in vec2 TexCoords;

uniform sampler2D baseImage;
uniform sampler2D normalMap; // in view space
uniform sampler2D ssrValuesMap;
uniform sampler2D positionMap; // in view space
uniform mat4 projection, view;

uniform vec3 skyColor = vec3(0.0);
uniform int binarySearchCount = 10;
uniform int rayMarchCount = 30; // 60
uniform float rayStep = 0.05; // 0.025
uniform float LLimiter = 0.1;
uniform float minRayStep = 0.2;

in vec2 texCoord;

// SSR based on tutorial by Imanol Fotia
// http://imanolfotia.com/blog/update/2017/03/11/ScreenSpaceReflections.html

#define getPosition(texCoord) texture(positionMap, texCoord).xyz

vec2 binarySearch(inout vec3 dir, inout vec3 hitCoord, inout float dDepth) {
    float depth;

    vec4 projectedCoord;
 
    for (int i = 0; i < binarySearchCount; i++) {
        projectedCoord = projection * vec4(hitCoord, 1.0);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
 
        depth = getPosition(projectedCoord.xy).z;
 
        dDepth = hitCoord.z - depth;

        dir *= 0.5;

        if (dDepth > 0.0) {
            hitCoord += dir;
        } else {
            hitCoord -= dir;
        }
    }

    projectedCoord = projection * vec4(hitCoord, 1.0);
    projectedCoord.xy /= projectedCoord.w;
    projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
 
    return vec2(projectedCoord.xy);
}

vec2 rayCast(vec3 dir, inout vec3 hitCoord, out float dDepth) {
    dir *= rayStep;
    
    for (int i = 0; i < rayMarchCount; i++) {
        hitCoord += dir;

        vec4 projectedCoord = projection * vec4(hitCoord, 1.0);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5; 

        float depth = getPosition(projectedCoord.xy).z;

        dDepth = hitCoord.z - depth;

        if ((dir.z - dDepth) < 1.2 && dDepth <= 0.0) {
            return binarySearch(dir, hitCoord, dDepth);
        }
    }

    return vec2(-1.0);
}

#define scale vec3(.8, .8, .8)
#define k 19.19

vec3 hash(vec3 a) {
    a = fract(a * scale);
    a += dot(a, a.yxz + k);
    return fract((a.xxy + a.yxx)*a.zyx);
}

// source: https://www.standardabweichung.de/code/javascript/webgl-glsl-fresnel-schlick-approximation
#define fresnelExp 5.0

float fresnel(vec3 direction, vec3 normal) {
    vec3 halfDirection = normalize(normal + direction);
    
    float cosine = dot(halfDirection, direction);
    float product = max(cosine, 0.0);
    float factor = 1.0 - pow(product, fresnelExp);
    
    return factor;
}

void mai3n()
{
    fragColor = vec3(1, 0, 0);
}

void main() {
    
     // position from depth
    vec2 gScreenSize = vec2(screenWidth, screenHeight);
    vec2 TexCoord = gl_FragCoord.xy / gScreenSize;
    float z = texture(DEPTH_TEXTURE, vec2(texCoord.s, texCoord.t)).x * 2.0f - 1.0f;
    vec4 clipSpacePosition = vec4(vec2(texCoord.s, texCoord.t) * 2.0 - 1.0, z, 1.0);
    vec4 viewSpacePosition = inverseProjectionMatrix * clipSpacePosition;    
    viewSpacePosition /= viewSpacePosition.w;
    vec4 worldSpacePosition = inverseViewMatrix * viewSpacePosition;
    vec3 worldPos = worldSpacePosition.xyz;

    float SSR_Mask =  texture(EMISSIVE_TEXTURE, vec2(texCoord.s, texCoord.t)).a;
    float Metallic = texture(RMA_TEXTURE, vec2(texCoord.s, texCoord.t)).g;

    //if(Metallic < 0.01)
    //    discard;
 
    if(SSR_Mask > 0.5)
        discard;



    float yIncrement = 1 / screenHeight;

    int counter = 0;

    vec3 color = vec3(1,0,0);

    for (int i=0; i < 25; i++)
    {

        float yTest = texCoord.t - (yIncrement * i);

        //float yTestCoord = texCoord.t - (yIncrement * v);
        
        //float SSR_Mask_Test =  texture(EMISSIVE_TEXTURE, vec2(texCoord.s, yTestCoord)).a;
        
        float pixelAbove = texture(EMISSIVE_TEXTURE, vec2(texCoord.s, yTest + yIncrement)).a;
        float pixelBelow = texture(EMISSIVE_TEXTURE, vec2(texCoord.s, yTest - yIncrement)).a;
        
        if (pixelAbove < 0.5)        
            if (pixelBelow > 0.5)
//              color.g += (1.0 / 256);
             color.g = 1;
        //   color =  texture(PREVIOUS_FRAME_TEXTURE, vec2(texCoord.s, yTest)).rgb;

     //   float yTestCoord = texCoord.t - (yIncrement * 10);
 

     //   if(SSR_Mask_Test < 0.5) {

      //if (yTestCoord > 0)
         //   color.r += 0.001;
           // color.r += (1.0 / 256.0);
           // break;
     //   }
       // }
        
       // color.r = SSR_Mask;
    }

 //   yIncrement *= 10;

    //float pixelBelow = texture(EMISSIVE_TEXTURE, vec2(texCoord.s, texCoord.t + yIncrement)).a;
    
       // float yTestCoord = texCoord.t - (yIncrement * 10);
      //  vec3 pixelAbove = texture(PREVIOUS_FRAME_TEXTURE, vec2(texCoord.s, yTestCoord)).rgb;



//color.rgb = vec3(1, 0, 0);

     fragColor = vec3(color);
    // fragColor = vec3(pixelAbove);


    //vec3 viewNormal = vec3(texture2D(NORMAL_TEXTURE, texCoord) * inverseViewMatrix);
    //vec3 viewPos = textureLod(gPosition, TexCoords, 2).xyz;
    //vec3 albedo = texture(gFinalImage, TexCoords).rgb;

    vec3 worldNormal = texture2D(NORMAL_TEXTURE, texCoord).rgb;


    //float spec = texture(ColorBuffer, TexCoords).w;
    //
    //vec3 F0 = vec3(0.04); 
    //F0      = mix(F0, albedo, Metallic);
    //vec3 Fresnel = fresnelSchlick(max(dot(normalize(viewNormal), normalize(viewPos)), 0.0), F0);

    // Reflection vector
    //vec3 reflected = normalize(reflect(normalize(viewPos), normalize(viewNormal)));
    vec3 reflected = normalize(reflect(normalize(worldPos), normalize(worldNormal)));
  //  fragColor = reflected;

    vec3 hitPos = worldPos;
    float dDepth;

   // fragColor = vec3(1, 0, 0);


 //   fragColor = vec3(SSR_Mask);
 
  //  vec3 wp = vec3(vec4(viewPos, 1.0) * invView);
  //  vec3 jitt = mix(vec3(0.0), vec3(hash(wp)), spec);
 //   vec4 coords = RayMarch((vec3(jitt) + reflected * max(minRayStep, -viewPos.z)), hitPos, dDepth);
 
 
  //  vec2 dCoords = smoothstep(0.2, 0.6, abs(vec2(0.5, 0.5) - coords.xy));
 
 
   // float screenEdgefactor = clamp(1.0 - (dCoords.x + dCoords.y), 0.0, 1.0);

  //  float ReflectionMultiplier = pow(Metallic, reflectionSpecularFalloffExponent) * screenEdgefactor * -reflected.z;
 
    // Get color
    //vec3 SSR = textureLod(gFinalImage, coords.xy, 0).rgb * clamp(ReflectionMultiplier, 0.0, 0.9) * Fresnel;  

    //outColor = vec4(SSR, Metallic);
}
