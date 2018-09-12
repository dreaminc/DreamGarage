// screenfade.frag
// shadertype=glsl

// This is a minimal shader that simply displays a texture to screen quad

in Data {
	vec4 color;
	vec2 uvCoord;
} DataIn;

layout (binding = 0) uniform sampler2D u_textureColor;
layout (binding = 1) uniform sampler2DMS u_textureColorMS;
uniform int u_textureColorMS_n;

uniform vec4 u_vec4FadeColor;
uniform float u_fadeProgress;

uniform	bool u_fTextureMS;
bool g_fTextureMS = true;

layout (location = 0) out vec4 out_vec4Color;

uniform vec4 u_vec4BackgroundColor;
uniform float u_windowWidth;
uniform float u_windowHeight;

void main(void) {  
	// Look up texture by coord
	vec4 color = vec4(0.0f);

	//TODO: this could be removed, but this shader takes the same uniforms as screenquad so don't want to cause problems
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
	
	// sigmoid(3.0f, 1.0f) = 0.9526f, so we need to scale it for a pure black during the fade
	// may make sense to not expose the rate in this way anymore
	float scale = 1.0f / 0.9526f;
	float sigmoidProgress = sigmoid(3.0f, u_fadeProgress) * scale;
	if (sigmoidProgress < 0.05f) {
		out_vec4Color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	}
	else {
		//out_vec4Color = (1.0f - sigmoidProgress) * color + sigmoidProgress * u_vec4FadeColor;
		out_vec4Color = (sigmoidProgress) * color + (1.0f - sigmoidProgress) * u_vec4FadeColor;
	}
}
