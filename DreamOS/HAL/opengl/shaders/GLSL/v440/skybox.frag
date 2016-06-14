// minimal.vert
// shadertype=glsl
#version 440 core

in vec3 inF_vec3Color;

in Data {
	vec4 color;
	vec2 uvCoord;
	vec3 position;
	vec4 vertWorldSpace;
	vec4 vertViewSpace;
} DataIn;

layout (location = 0) uniform sampler2D u_textureColor;
layout (location = 1) uniform sampler2D u_textureBump;
layout (location = 2) uniform samplerCube u_textureCubeMap;

layout (location = 0) out vec4 out_vec4Color;

void main(void) {  	
	out_vec4Color = texture(u_textureCubeMap, DataIn.position);;
}