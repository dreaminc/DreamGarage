// minimal.vert
// shadertype=glsl

// This is a minimal shader that simply displays
// a vertex color and position (no lighting, no textures)

#version 440 core

in Data {
	vec4 color;
} DataIn;

struct Material {
	float m_shine;
	float m_bumpiness;
	float m_ambient;
	float reserved3;
	vec4 m_colorAmbient;
	vec4 m_colorDiffuse;
	vec4 m_colorSpecular;
	float m_tilingU;
	float m_tilingV;
	float reserved1;
	float reserved2;
};

layout(std140) uniform ub_material {
    Material material;
};

layout (location = 0) out vec4 out_vec4Color;

void main(void) {  
	out_vec4Color = DataIn.color;
}