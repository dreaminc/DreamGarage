// minimal.vert
//#version 150 core
#version 450 core

layout (location = 0) in vec3 inV_vec3Position;
layout (location = 1) in vec3 inV_vec3Color;

uniform mat4 u_mat4Model;
uniform mat4 u_mat4View;
uniform mat4 u_mat4Projection;

uniform mat4 u_mat4ModelViewProjection;

out vec3 inF_vec3Color;

void main(void)
{
      //gl_Position = u_mat4Projection * u_mat4View * u_mat4Model * vec4(inV_vec3Position, 1.0);
	  gl_Position = u_mat4ModelViewProjection * vec4(inV_vec3Position, 1.0);

      inF_vec3Color = inV_vec3Color;
}