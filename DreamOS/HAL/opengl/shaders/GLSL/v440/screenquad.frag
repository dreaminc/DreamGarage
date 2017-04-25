// screenquad.vert
// shadertype=glsl

// This is a minimal shader that simply displays a texture to screen quad

#version 440 core

in Data {
	vec4 color;
} DataIn;

layout (location = 0) out vec4 out_vec4Color;

uniform sampler2D u_textureColor;
uniform vec4 u_vec4BackgroundColor;

void main(void) {  
	// Look up texture by coord
	vec4 color = texture(u_textureColor, gl_FragCoord.xy);

	// Add to background color
	//out_vec4Color = DataIn.color + u_vec4BackgroundColor * DataIn.color.a;
	out_vec4Color = vec4(1.0f);
}