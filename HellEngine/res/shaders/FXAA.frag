#version 130
//#extension GL_EXT_gpu_shader4: enable
uniform sampler2D tex0; 

in vec2 TexCoords;
out vec4 FragColor;

//uniform sampler2D buf0;
//uniform vec2 frameBufSize;
//varying vec2 texCoords;

void main( void ) {
    //gl_FragColor.xyz = texture2D(buf0,texCoords).xyz;
    //return;

  vec2 texCoords = vec2(TexCoords.x, TexCoords.y);

	vec2 texCoordOffset = 1.0f / textureSize(tex0,0);
	float fxaaSpanMax = 8.0f;
	float fxaaReduceMin = 1.0f/128.0f;
	float fxaaReduceMul = 1.0f/8.0f;


	vec3 luma = vec3(0.299, 0.587, 0.114);	
	float lumaTL = dot(luma, texture2D(tex0, texCoords + (vec2(-1.0, -1.0) * texCoordOffset)).xyz);
	float lumaTR = dot(luma, texture2D(tex0, texCoords + (vec2(1.0, -1.0) * texCoordOffset)).xyz);
	float lumaBL = dot(luma, texture2D(tex0, texCoords + (vec2(-1.0, 1.0) * texCoordOffset)).xyz);
	float lumaBR = dot(luma, texture2D(tex0, texCoords + (vec2(1.0, 1.0) * texCoordOffset)).xyz);
	float lumaM  = dot(luma, texture2D(tex0, texCoords).xyz);

	vec2 dir;
	dir.x = -((lumaTL + lumaTR) - (lumaBL + lumaBR));
	dir.y = ((lumaTL + lumaBL) - (lumaTR + lumaBR));
	
	float dirReduce = max((lumaTL + lumaTR + lumaBL + lumaBR) * (fxaaReduceMul * 0.25), fxaaReduceMin);
	float inverseDirAdjustment = 1.0/(min(abs(dir.x), abs(dir.y)) + dirReduce);
	
	dir = min(vec2(fxaaSpanMax, fxaaSpanMax), 
		max(vec2(-fxaaSpanMax, -fxaaSpanMax), dir * inverseDirAdjustment)) * texCoordOffset;

	vec3 result1 = (1.0/2.0) * (
		texture2D(tex0, texCoords + (dir * vec2(1.0/3.0 - 0.5))).xyz +
		texture2D(tex0, texCoords + (dir * vec2(2.0/3.0 - 0.5))).xyz);

	vec3 result2 = result1 * (1.0/2.0) + (1.0/4.0) * (
		texture2D(tex0, texCoords + (dir * vec2(0.0/3.0 - 0.5))).xyz +
		texture2D(tex0, texCoords + (dir * vec2(3.0/3.0 - 0.5))).xyz);

	float lumaMin = min(lumaM, min(min(lumaTL, lumaTR), min(lumaBL, lumaBR)));
	float lumaMax = max(lumaM, max(max(lumaTL, lumaTR), max(lumaBL, lumaBR)));
	float lumaResult2 = dot(luma, result2);
	
	if(lumaResult2 < lumaMin || lumaResult2 > lumaMax)
		FragColor = vec4(result1, 1.0);
	else
		FragColor = vec4(result2, 1.0);
 //   FragColor.x = 1;
 // FragColor = vec4(vec3(texture(tex0, TexCoords)), 1.0);
}
 //gl_FragColor = vec4(1, 0, 1, 1.0);
