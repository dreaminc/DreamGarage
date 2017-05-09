// screenquad.vert
// shadertype=glsl

// This is a minimal shader that simply displays a texture to screen quad

#version 440 core

in Data {
	vec4 color;
	vec2 uvCoord;
} DataIn;

//uniform sampler2DRect u_textureColor;
uniform sampler2D u_textureColor;

layout (location = 0) out vec4 out_vec4Color;

uniform vec4 u_vec4BackgroundColor;

void main(void) {  
	// Look up texture by coord
	//vec4 color = texture(u_textureColor, gl_FragCoord.xy);
	vec4 color = texture(u_textureColor, DataIn.uvCoord * 1.0f);
	
	//float param = color.x;

	// Add to background color
	//out_vec4Color = color + u_vec4BackgroundColor * DataIn.color.a;
	//out_vec4Color = vec4(param, param, param, 1.0f);
	out_vec4Color = color;
}