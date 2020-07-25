#version 330 core

layout (location = 0) out vec3 buffer0;
//layout (location = 1) out vec3 buffer1;

uniform int bufferIndex;
uniform vec3 color;

void main()
{   
   // if (bufferIndex == 0)
        buffer0 = color;

  //  if (bufferIndex == 1)
 //       buffer1 = color;
}