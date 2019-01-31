// billboard.frag
// shadertype=glsl

in GS_OUT {
	vec2 uvCoord;
} gs_out;

layout (binding = 0) uniform sampler2D u_textureColor;

void main(void) {
	vec4 color = texture(u_textureColor, gs_out.uvCoord);

	out vec4Color = color;
}
