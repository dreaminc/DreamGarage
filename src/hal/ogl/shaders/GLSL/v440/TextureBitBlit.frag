// minimalTexture.vert
// shadertype=glsl

// This is a minimal texture shader that simply displays
// a vertex color and position (no lighting)

#version 440 core

in Data {
	vec4 color;
	vec2 uvCoord;
} DataIn;

uniform	int u_hasTexture;
uniform sampler2D u_textureColor;

layout (location = 0) out vec4 out_vec4Color;

void main(void) {  
	vec4 textureColor = texture(u_textureColor, DataIn.uvCoord * 1.0f);

	if (u_hasTexture == 1) {
		out_vec4Color = textureColor;
	}
	else {
		out_vec4Color = DataIn.color;
	}
}