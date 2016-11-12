#version 400 core

in Data {
	vec4 color;
	vec2 uvCoord;
} DataIn;

uniform	int u_hasTexture;
uniform sampler2D u_textureColor;

uniform bool u_fDistanceMap;
uniform float u_buffer;
uniform float u_gamma;

layout (location = 0) out vec4 out_vec4Color;

vec4 g_ambient = vec4(0.1);

void main(void) {  
	vec4 textureColor = texture(u_textureColor, DataIn.uvCoord * 1.0f);
	if (u_hasTexture == 1) {
		out_vec4Color = textureColor;
	}
	else {
		out_vec4Color = DataIn.color;
	}

	// Distance mapping

	if (u_fDistanceMap) {
		float dist = texture(u_textureColor, DataIn.uvCoord * 1.0f).a;
		float alpha = smoothstep(u_buffer - u_gamma, u_buffer + u_gamma, dist);
		out_vec4Color = vec4(out_vec4Color.rgb, alpha * out_vec4Color.a/u_buffer);
	}
}
