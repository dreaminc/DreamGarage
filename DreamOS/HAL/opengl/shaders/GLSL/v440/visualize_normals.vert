// visualize_normals.geo
// shadertype=glsl

// This is a minimal vertex shader for the visualization of normals program

#version 440 core

layout (location = 0) in vec4 inV_vec4Position;
layout (location = 1) in vec4 inV_vec4Color;

out Data {
	vec4 color;
} DataOut;

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

uniform mat4 u_mat4Model;
uniform mat4 u_mat4ViewProjection;

void main(void) {	
	// Vert Color
	//DataOut.color = inV_vec4Color;
	DataOut.color = material.m_colorSpecular;

	// Projected Vert Position
	gl_Position = u_mat4ViewProjection * u_mat4Model * vec4(inV_vec4Position.xyz, 1.0f);
}