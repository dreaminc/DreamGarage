// materialCommon.shader
// shadertype=glsl

// A shared GLSL library for material functionality

struct Material {
	float m_shine;
	float m_bumpiness;
	float m_ambient;
	float m_displacement;
	vec4 m_colorAmbient;
	vec4 m_colorDiffuse;
	vec4 m_colorSpecular;
	float m_tilingU;
	float m_tilingV;
	float m_reflectivity;
	float m_refractivity;
};

layout(std140) uniform ub_material{
	Material material;
};