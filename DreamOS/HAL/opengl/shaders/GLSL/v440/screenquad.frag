// screenquad.vert
// shadertype=glsl

// This is a minimal shader that simply displays a texture to screen quad

#version 440 core

in Data {
	vec4 color;
} DataIn;

uniform sampler2D u_textureColor;

layout (location = 0) out vec4 out_vec4Color;

uniform vec4 u_vec4BackgroundColor;

void main(void) {  
	// Look up texture by coord
	vec4 color = texture(u_textureColor, (gl_FragCoord.xy + 1.0f)/100.0f);

	// Add to background color
	//out_vec4Color = color + u_vec4BackgroundColor * DataIn.color.a;
	out_vec4Color = color;
}