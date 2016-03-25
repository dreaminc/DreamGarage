// minimal.vert
// shadertype=glsl

#version 440 core

#define MAX_TOTAL_LIGHTS 10

// TODO: Move to a uniform block
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

void CalculateVertexLightValue(in Light light, in vec4 vertWorldSpace, in vec4 vectorNormal, out float lightValue) {
	vec3 directionLight = vec3(light.m_ptOrigin - vertWorldSpace);
	vec3 directionEye = vec3(-vertWorldSpace);
	
	vec3 halfVector = directionLight + directionEye;
	float specularComponent = 

	float distanceLight = length(directionLight);
	
	vec4 vec4ModelNormal = mat4InvTransposeModel * vectorNormal;

	float cosThetaOfLightToVert = max(dot(vec3(vec4ModelNormal), directionLight), 0.0);
	
	lightValue = (light.m_power / (distanceLight * distanceLight)) * cosThetaOfLightToVert;
}

void main(void) {	
	vec4 vertWorldSpace = u_mat4Model * inV_vec4Position;

	vec3 vec3LightValue = vec3(0.0f, 0.0f, 0.0f);
	float activeLightValue = 0.0f;

	for(int i = 0; i < numLights; i++) {
		CalculateVertexLightValue(lights[i], vertWorldSpace, inV_vec4Normal, activeLightValue);
		vec3LightValue += activeLightValue * vec3(lights[i].m_colorDiffuse);
	}

	// Projected Vert Position
	gl_Position = u_mat4ViewProjection * vertWorldSpace;

	// Vert Color
	inF_vec3Color = (vec3LightValue * vec3(inV_vec4Color)) + vec3(g_vec4AmbientLightLevel);
}