// ssao.frag
// shadertype=glsl

// This is a minimal shader that simply displays a texture to screen quad

in Data {
	vec4 color;
	vec2 uvCoord;
} DataIn;

layout (binding = 0) uniform sampler2D u_textureColor;
layout (binding = 1) uniform sampler2DMS u_textureColorMS;
uniform int u_textureColorMS_n;

uniform	bool u_fTextureMS;
bool g_fTextureMS = true;

layout (location = 0) out vec4 out_vec4Color;

uniform vec4 u_vec4BackgroundColor;
uniform float u_windowWidth;
uniform float u_windowHeight;

void main(void) {  
	// Look up texture by coord
	vec4 color = vec4(0.0f);

	if(u_fTextureMS == true) {
		vec4 colorAccumulator = vec4(0.0f);
		for(int i = 0; i < u_textureColorMS_n; i++) {
			colorAccumulator += texelFetch(u_textureColorMS, ivec2(gl_FragCoord.xy), i);
		}
		
		//color = texelFetch(u_textureColorMS, ivec2(gl_FragCoord.xy), 1);
		color = colorAccumulator / (u_textureColorMS_n);
	}
	else {
		//color = texture(u_textureColor, DataIn.uvCoord * 1.0f);

		vec2 inverseScreenSize = vec2(1.0f / u_windowWidth, 1.0f / u_windowHeight);
		color = GetFXAAColor(u_textureColor, DataIn.uvCoord, inverseScreenSize);
	}
	
	out_vec4Color = color;
}