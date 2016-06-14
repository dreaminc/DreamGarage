// minimal.vert
// shadertype=glsl

// This is a minimal shader that simply displays
// a vertex color and position (no lighting, no textures)

#version 400 core

layout (location = 0) in vec4 inV_vec4Position;
layout (location = 1) in vec4 inV_vec4Color;

out Data {
	vec4 color;
} DataOut;

uniform mat4 u_mat4Model;
uniform mat4 u_mat4ViewProjection;

void main(void) {	
	// Vert Color
	DataOut.color = inV_vec4Color;

	// Projected Vert Position
	gl_Position = u_mat4ViewProjection * u_mat4Model * vec4(inV_vec4Position.xyz, 1.0f);
}