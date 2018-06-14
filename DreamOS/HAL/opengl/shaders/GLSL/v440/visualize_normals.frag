// visualize_normals.vert
// shadertype=glsl

// This is a minimal fragment shader for the visualization of normals
// program

#version 440 core

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

void main(void) {  
	out_vec4Color = vec4(1.0, 1.0, 0.0, 1.0);
}