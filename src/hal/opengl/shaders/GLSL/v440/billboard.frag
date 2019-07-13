// billboard.frag
// shadertype=glsl

in GS_OUT {
	vec2 uvCoord;
} gs_out;

layout (binding = 0) uniform sampler2D u_textureColor;

layout (location = 0) out vec4 out_vec4Color;

uniform bool	u_hasTextureColor;

void main(void) {

	vec4 color = vec4(1.0f);

	if (u_hasTextureColor) {
		color = texture(u_textureColor, gs_out.uvCoord);
	}

	out_vec4Color = color;
}
