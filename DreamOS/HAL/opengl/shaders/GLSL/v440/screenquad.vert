// screenquad.vert
// shadertype=glsl

// This is a minimal shader that simply renders
// a texture to scren

layout (location = 0) in vec4 inV_vec4Position;
layout (location = 1) in vec4 inV_vec4Color;
layout (location = 3) in vec2 inV_vec2UVCoord;

out Data {
	vec4 color;
	vec2 uvCoord;
} DataOut;

void main(void) {	
	// UV Coordinate
	DataOut.uvCoord = inV_vec2UVCoord;

	gl_Position = vec4(inV_vec4Position.xy * 2.0f, 0.0f , 1.0f);
}