// screenquad.vert
// shadertype=glsl

// This is a minimal shader that simply renders
// a texture to scren

#version 440 core

out Data {
	vec4 color;
} DataOut;

layout (location = 0) in vec4 inV_vec4Position;

void main(void) {	
	gl_Position = vec4(inV_vec4Position.xy * 2.0f, 0.0f , 1.0f);
}