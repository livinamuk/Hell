#version 330

layout (location = 0) in vec3 Position; 

uniform mat4 VP;
uniform mat4 model;

void main()
{          
    gl_Position = VP * model * vec4(Position, 1.0);
}
