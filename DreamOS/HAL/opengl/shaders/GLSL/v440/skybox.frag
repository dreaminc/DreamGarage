// minimal.vert
// shadertype=glsl

in vec3 inF_vec3Color;

in Data {
	vec4 color;
	vec2 uvCoord;
	vec3 position;
	vec4 vertWorldSpace;
	vec4 vertViewSpace;
} DataIn;

uniform samplerCube u_textureCubeMap;

layout (location = 0) out vec4 out_vec4Color;

void main(void) {  	
	out_vec4Color = texture(u_textureCubeMap, DataIn.position);
}