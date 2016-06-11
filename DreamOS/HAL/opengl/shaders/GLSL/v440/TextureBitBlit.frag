// minimalTexture.vert
// shadertype=glsl

// This is a minimal texture shader that simply displays
// a vertex color and position (no lighting)

#version 440 core

in Data {
	vec4 color;
	vec2 uvCoord;
} DataIn;

uniform sampler2D u_textureColor;

layout (location = 0) out vec4 out_vec4Color;

vec4 g_ambient = vec4(0.1);

void main(void) {  
	vec4 textureColor = texture(u_textureColor, DataIn.uvCoord * 1.0f);
	g_ambient;
	out_vec4Color = textureColor;
}