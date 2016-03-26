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
	float m_shine;
	float reserved2;

	vec4 m_ptOrigin;
	vec4 m_colorDiffuse;
	vec4 m_colorSpecular;
	vec4 m_vectorDirection; 
};

struct Material {
    float m_shine;
	float reserved1;
	float reserved2;
	float reserved3;
    vec4 m_colorAmbient;
    vec4 m_colorDiffuse;
    vec4 m_colorSpecular;
};

layout(std140) uniform ub_LightArray {
	Light lights[MAX_TOTAL_LIGHTS];
	int numLights;	
};

Material g_mat = {
	5.0f,	// shine
	0.0f,
	0.0f,
	0.0f,
	vec4(1.0f, 1.0f, 1.0f, 1.0f),	// ambient
	vec4(1.0f, 1.0f, 1.0f, 1.0f),	// diffuse
	vec4(1.0f, 1.0f, 1.0f, 1.0f)	// specular
};

vec4 g_vec4AmbientLightLevel = 1.0 * vec4(1.0, 1.0, 1.0, 0.0);

// TODO: Move to CPU side
mat4 mat4InvTransposeModel = transpose(inverse(u_mat4Model));

void CalculateVertexLightValue(in Light light, in vec4 vertWorldSpace, in vec4 vectorNormal, out float diffuseValue, out float specularValue) {
	float distanceLight = length(vec3(light.m_ptOrigin - vertWorldSpace));
	vec3 directionLight = vec3(light.m_ptOrigin - vertWorldSpace) / distanceLight;
		
	vec4 vec4ModelNormal = normalize(mat4InvTransposeModel * vectorNormal);

	float cosThetaOfLightToVert = max(dot(vec3(vec4ModelNormal), directionLight), 0.0);
	diffuseValue = (light.m_power / (distanceLight * distanceLight)) * cosThetaOfLightToVert;

	if(diffuseValue > 0.0) {
		vec3 directionEye = vec3(normalize(-vertWorldSpace));
		vec3 halfVector = normalize(directionLight + directionEye);
		specularValue = pow(max(dot(halfVector, vec3(vec4ModelNormal)), 0.0), g_mat.m_shine);
	}
	else {
		specularValue = 0.0f;
	}
}

void main(void) {	
	vec4 vertWorldSpace = u_mat4Model * inV_vec4Position;

	vec3 vec3LightValue = vec3(0.0f, 0.0f, 0.0f);
	float diffuseValue = 0.0f, specularValue = 0.0f;

	for(int i = 0; i < numLights; i++) {
		CalculateVertexLightValue(lights[i], vertWorldSpace, inV_vec4Normal, diffuseValue, specularValue);
		vec3LightValue += diffuseValue * vec3(lights[i].m_colorDiffuse);
		vec3LightValue += specularValue * vec3(lights[i].m_colorSpecular);
	}

	// Projected Vert Position
	gl_Position = u_mat4ViewProjection * vertWorldSpace;

	// Vert Color
	inF_vec3Color = (vec3LightValue * vec3(inV_vec4Color)) + vec3(g_vec4AmbientLightLevel);
}