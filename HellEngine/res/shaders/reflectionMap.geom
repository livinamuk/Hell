#version 400 core
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

//uniform int index;
uniform mat4 captureViewMatrix[6];

in vec3 gNormal[];
in vec2 gTexCoord[];

out vec4 FragPos;
out vec2 TexCoord;
out vec3 Normal;

void main()
{
    for(int face = 0; face < 6; ++face)
    {
        gl_Layer = face; 
		// for each triangle's vertices
        for(int i = 0; i < 3; ++i)         
		{
            FragPos = gl_in[i].gl_Position;
			
			TexCoord = gTexCoord[i];
			Normal = gNormal[i];

            gl_Position = captureViewMatrix[face] * FragPos;
            EmitVertex();
        }    
        EndPrimitive();
    }
} 