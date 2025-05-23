// minimalTexture.vert
// shadertype=glsl

// This is a minimal texture shader that simply displays
// a vertex color and position (no lighting)

in Data {
	vec4 color;
	vec2 uvCoord;
	vec4 vertWorldSpace;
	float vertDepth;
} DataIn;

uniform bool u_hasTextureColor;
layout (binding = 0) uniform sampler2D u_textureColor;

layout (location = 0) out vec4 out_vec4Color;

//vec4 g_ambient = vec4(0.05f);
vec4 g_ambient = vec4(0.0f);

uniform mat4 u_mat4View;

void main(void) {  
	vec4 textureColor = vec4(1.0f);

	if (u_hasTextureColor == true) {
		textureColor = texture(u_textureColor, DataIn.uvCoord);
	}
	else {
		textureColor = DataIn.color;
	}
	
	vec4 shaderColor = material.m_colorDiffuse * textureColor + g_ambient;

	
	// TODO: Not really the best way to do this since now minimal texture program
	// needs a special uniform
	out_vec4Color = MixWithFog(FOG_TYPE_LINEAR, shaderColor, DataIn.vertDepth);
}