// minimalTexture.vert
// shadertype=glsl

// This is a minimal texture shader that simply displays
// a vertex color and position (no lighting)

#version 440 core

in Data {
	vec4 color;
	vec2 uvCoord;
} DataIn;

uniform sampler2D u_textureColor;

struct Material {
	float m_shine;
	float m_bump;
	float m_ambient;
	float reserved3;
    vec4 m_colorAmbient;
    vec4 m_colorDiffuse;
    vec4 m_colorSpecular;
};

layout(std140) uniform ub_material {
    Material material;
};

layout (location = 0) out vec4 out_vec4Color;

//vec4 g_ambient = vec4(0.05f);
vec4 g_ambient = vec4(0.0f);

void main(void) {  
	vec4 textureColor = texture(u_textureColor, DataIn.uvCoord);
	out_vec4Color = material.m_colorDiffuse * textureColor + g_ambient;
	//out_vec4Color = material.m_colorDiffuse;
	//out_vec4Color = DataIn.color * textureColor + g_ambient;
}