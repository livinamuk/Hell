#version 420 core

layout (location = 0) out vec3 CompositeALB;
layout (location = 1) out vec3 CompositeRMA;

layout (binding = 0) uniform sampler2D ALB_TEX;
layout (binding = 1) uniform sampler2D RMA_TEX;
layout (binding = 2) uniform sampler2D BLOOD_TEX;

in vec2 TexCoords;

//uniform sampler2D Emissive_Texture;
//uniform float contrast;

void main()
{
	vec3 ALB = vec3(texture(ALB_TEX, TexCoords));
	vec3 RMA = vec3(texture(RMA_TEX, TexCoords));
	vec3 BLOOD = vec3(texture(BLOOD_TEX, TexCoords));
	
	float red = 0.45;

	vec3 newBLOOD = vec3(1) - BLOOD;
	newBLOOD.r -= red;	
	newBLOOD = vec3(1) - newBLOOD;
	
	newBLOOD.r = min(1, newBLOOD.r);
	newBLOOD.g = min(1, newBLOOD.g);
	newBLOOD.b = min(1, newBLOOD.b);
	newBLOOD.r = max(0, newBLOOD.r);
	newBLOOD.g = max(0, newBLOOD.g);
	newBLOOD.b = max(0, newBLOOD.b);
	
	vec3 color = RMA = RMA * BLOOD;
	CompositeALB = vec3(ALB * newBLOOD);

	// Rma
	vec3 inverseBloodMask = 1 - BLOOD;
	//CompositeRMA = (RMA * BLOOD) + inverseBloodMask * vec3(0.41, 0.69, 0.97);
	CompositeRMA = (RMA * BLOOD) + inverseBloodMask * vec3(0.15, 0.0, 0);
	
	//CompositeALB = vec3(texture(ALB_TEX, TexCoords)) + vec3(texture(BLOOD_TEX, TexCoords)); ;
	CompositeALB = vec3(texture(ALB_TEX, TexCoords));

	//CompositeALB = ALB;// *  vec3(  1 - texture(BLOOD_TEX, TexCoords).r  ) + BLOOD;
	CompositeRMA =  vec3(texture(RMA_TEX, TexCoords));;

	CompositeRMA.r = clamp(CompositeRMA.r , 0, 1);
	CompositeRMA.g = clamp(CompositeRMA.g , 0, 1);
	CompositeRMA.b = clamp(CompositeRMA.b , 0, 1);	
	
	//CompositeALB.r = clamp(CompositeALB.r , 0, 1);
	//CompositeALB.g = clamp(CompositeALB.g , 0, 1);
	//CompositeALB.b = clamp(CompositeALB.b , 0, 1);

	//CompositeRMA *= 0.5;
//	CompositeALB =   vec3(  1 - texture(BLOOD_TEX, TexCoords).r  );
}