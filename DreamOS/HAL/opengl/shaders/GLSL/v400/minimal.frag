// minimal.vert
// shadertype=glsl

// This is a minimal shader that simply displays
// a vertex color and position (no lighting, no textures)

#version 400 core

in Data {
	vec4 color;
} DataIn;

layout (location = 0) out vec4 out_vec4Color;

void main(void) {  
	out_vec4Color = DataIn.color;
}