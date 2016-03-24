// minimal.vert
// shadertype=glsl

#version 440 core

#define MAX_TOTAL_LIGHTS 2

layout (location = 0) in vec4 inV_vec4Position;
layout (location = 1) in vec4 inV_vec4Color;
layout (location = 2) in vec4 inV_vec4Normal;

out vec3 inF_vec3Color;

uniform mat4 u_mat4Model;
uniform mat4 u_mat4ViewProjection;

// Light Structure
struct Light {
	int m_type;
	float m_power;
	float reserved1;
	float reserved2;

	vec4 m_ptOrigin;
	vec4 m_colorDiffuse;
	vec4 m_colorSpecular;
	vec4 m_vectorDirection; 
};

layout(std140) uniform ub_LightArray {
	Light lights[MAX_TOTAL_LIGHTS];
	int numLights;	
};

Light g_lightTemp = Light(
	2,								// type
	1.0,							// Light Emit Power
	0,
	0,

	vec4(0.0, 5.0, 0.0, 1.0),		// origin
	vec4(1.0, 1.0, 1.0, 1.0),		// diffuse
	vec4(1.0, 1.0, 1.0, 1.0),		// specular
	vec4(0.0, -1.0, 0.0, 0.0)		// direction
);

vec4 g_vec4AmbientLightLevel = 0.05 * vec4(1.0, 1.0, 1.0, 0.0);

mat4 mat4InvTransposeModel = transpose(inverse(u_mat4Model));

void main(void) {	
	//Light activeLight = g_lightTemp;
	Light activeLight = lights[0];

	vec4 vertWorldSpace = u_mat4Model * inV_vec4Position;
	
	vec3 directionLight = vec3(activeLight.m_ptOrigin - vertWorldSpace);
	float distanceLight = length(directionLight);
	
	vec4 vec4ModelNormal = mat4InvTransposeModel * inV_vec4Normal;

	float cosThetaOfLightToVert = max(dot(vec3(vec4ModelNormal), directionLight), 0.0);
	float lightValue = (activeLight.m_power / (distanceLight * distanceLight)) * cosThetaOfLightToVert;

	// Projected Vert Position
	gl_Position = u_mat4ViewProjection * vertWorldSpace;

	// Vert Color
	inF_vec3Color = (lightValue * vec3(inV_vec4Color)) + vec3(g_vec4AmbientLightLevel);
}