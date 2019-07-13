// ShadowDepth.vert
// shadertype=glsl

// The shadow depth fragment shader outputs a given fragment's 
// depth

#version 440 core

layout (location = 0) out float out_fragmentDepth;

void main(void) {  
	// Not really needed, OpenGL does it anyway
    out_fragmentDepth = gl_FragCoord.z;
}