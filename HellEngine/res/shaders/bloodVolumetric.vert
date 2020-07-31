#version 330 core

layout (location = 0) in vec3 a_Position;
layout (location = 2) in vec2 a_Texcoord;

uniform mat4 u_MatrixProjection;
uniform mat4 u_MatrixView;
uniform mat4 u_MatrixWorld;
uniform mat4 u_MatrixInverseWorld;

uniform sampler2D u_PosTex;
uniform sampler2D u_NormTex;
uniform float u_BoundingMax;
uniform float u_BoundingMin;
uniform float u_Speed;
uniform int u_NumOfFrames;
uniform vec4 u_HeightOffset;
uniform float u_TimeInFrames;
uniform vec3 u_WorldSpaceCameraPos;

out vec3 v_WorldNormal;
out vec3 v_ViewDir;

float LinearToGammaSpaceExact (float value)
{
    if (value <= 0.0F)
        return 0.0F;
    else if (value <= 0.0031308F)
        return 12.92F * value;
    else if (value < 1.0F)
        return 1.055F * pow(value, 0.4166667F) - 0.055F;
    else
        return pow(value, 0.45454545F);
}

vec3 LinearToGammaSpace (vec3 linRGB)
{
   return vec3(LinearToGammaSpaceExact(linRGB.r), LinearToGammaSpaceExact(linRGB.g), LinearToGammaSpaceExact(linRGB.b));
}

void main() {

    float currentSpeed = u_Speed / u_NumOfFrames;

    vec2 coord = vec2(a_Texcoord.x, a_Texcoord.y - u_TimeInFrames);    

    vec4 texturePos = textureLod(u_PosTex, coord, 0);
    vec4 textureNorm = textureLod(u_NormTex, coord, 0);
    
    texturePos.xyz = LinearToGammaSpace(texturePos.xyz);
    textureNorm.xyz = LinearToGammaSpace(textureNorm.xyz);

    float expand = u_BoundingMax - u_BoundingMin;
    
    texturePos.xyz *= expand;
    texturePos.xyz += vec3(u_BoundingMin, u_BoundingMin, u_BoundingMin);

    texturePos.x *= -1;
    vec3 vertexPos = texturePos.xyz + vec3(u_HeightOffset.x, u_HeightOffset.y, u_HeightOffset.z);
                
    v_WorldNormal = textureNorm.xyz * 2 - vec3(1, 1, 1); 
    v_WorldNormal.x = -v_WorldNormal.x;
    
    v_ViewDir = (u_MatrixInverseWorld * vec4(u_WorldSpaceCameraPos, 1.0)).xyz - vertexPos;

    gl_Position = u_MatrixProjection * u_MatrixView * u_MatrixWorld * vec4(vertexPos, 1.0);
}