#version 330 core

out vec4 FragColor;
in vec3 FragPos;
in vec2 TexCoord;
in vec3 Normal;
uniform sampler2D texture0;

uniform int TEXTURE_FLAG;
uniform vec2 TEXTURE_SCALE;

void main()
{
	//////////////////////////////////////
	// Recalculate texture co-ordinates //
	//////////////////////////////////////

	vec2 finalTexCoords = TexCoord;

	// Floor
	if (TEXTURE_FLAG == 3)								
		finalTexCoords = vec2(FragPos.x, FragPos.z) * 0.4;		// Regular
	if (TEXTURE_FLAG == 4)								
		finalTexCoords = vec2(FragPos.z, FragPos.x) * 0.4;		// Rotate 90 degrees

	// Scaling
	finalTexCoords.x *= TEXTURE_SCALE.x;
	finalTexCoords.y *= TEXTURE_SCALE.y;
	
	////////////////////
	// Final lighting //
	////////////////////

	FragColor = texture(texture0, finalTexCoords);
}