#version 440 core

in Data {
	vec4 color;
	vec2 uvCoord;
} DataIn;

uniform	bool u_hasColorTexture;
uniform sampler2D u_textureColor;

uniform bool u_fDistanceMap;
uniform float u_buffer;
uniform float u_gamma;

layout (location = 0) out vec4 out_vec4Color;

vec4 g_ambient = vec4(0.1);

float bufferValue = 0.5f;
float gammaValue = 0.0125f;

void main(void) {  
	
	// Distance mapping
	if (u_fDistanceMap == true) {
		float dist = texture(u_textureColor, DataIn.uvCoord * 1.0f).a;
		float alpha = smoothstep(bufferValue - gammaValue, bufferValue + gammaValue, dist);
		out_vec4Color = vec4(out_vec4Color.rgb, alpha * (out_vec4Color.a / u_buffer));
	}
	else {
		if (u_hasColorTexture) {
			out_vec4Color = texture(u_textureColor, DataIn.uvCoord * 1.0f);
		}
		else {
			out_vec4Color = DataIn.color;
		}
	}
}
