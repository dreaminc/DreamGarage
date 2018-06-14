// materialCommon.shader
// shadertype=glsl

// A shared GLSL library for material functionality

struct Material {
	float m_shine;
	float m_bump;
	float m_ambient;
	float reserved3;
	vec4 m_colorAmbient;
	vec4 m_colorDiffuse;
	vec4 m_colorSpecular;
};

layout(std140) uniform ub_material{
	Material material;
};