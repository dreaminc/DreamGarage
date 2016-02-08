// minimal.vert
//#version 150 core
#version 450 core

in vec3 inV_vec3Position;
in vec3 inV_vec3Color;

out vec3 inF_vec3Color;

void main(void)
{
      gl_Position = vec4(inV_vec3Position, 1.0);
      inF_vec3Color = inV_vec3Color;
}