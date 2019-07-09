// ShadowDepth.vert
// shadertype=glsl

// The shadow depth vertex is extremely simple
// all it does is project a given vertex according to the given MVP 
// matrix and passes it to the fragment shader

#version 440 core

layout (location = 0) in vec4 inV_vec4Position;

uniform mat4 u_mat4Model;
uniform mat4 u_mat4ViewProjection;

void main(void) {	
	
	// Projected Vert Position
	gl_Position = u_mat4ViewProjection * u_mat4Model * vec4(inV_vec4Position.xyz, 1.0f);
}