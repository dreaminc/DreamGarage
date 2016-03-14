// minimal.frag
#version 440 core

// precision highp float;

in vec3 inF_vec3Color;

layout (location = 0) out vec4 out_vec4Color;

void main(void)
{
      out_vec4Color = vec4(inF_vec3Color, 1.0);
}