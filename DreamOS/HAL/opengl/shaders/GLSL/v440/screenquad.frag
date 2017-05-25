// screenquad.vert
// shadertype=glsl

// This is a minimal shader that simply displays a texture to screen quad

#version 440 core

in Data {
	vec4 color;
	vec2 uvCoord;
} DataIn;

uniform sampler2D u_textureColor;
uniform sampler2DMS u_textureColorMS;

uniform	bool u_fTextureMS;
bool g_fTextureMS = true;

layout (location = 0) out vec4 out_vec4Color;

uniform vec4 u_vec4BackgroundColor;

void main(void) {  
	// Look up texture by coord
	vec4 color = vec4(0.0f);

	if(u_fTextureMS == true) {
		color = texelFetch(u_textureColorMS, ivec2(gl_FragCoord.xy), 1);
	}
	else {
		color = texture(u_textureColor, DataIn.uvCoord * 1.0f);
	}
	
	out_vec4Color = color;
}