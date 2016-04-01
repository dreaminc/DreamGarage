// minimal.vert
// shadertype=glsl
#version 440 core

#define MAX_TOTAL_LIGHTS 10

in vec3 inF_vec3Color;

in Data {
    vec4 normal;
    vec3 directionEye;
	vec3 directionLight[MAX_TOTAL_LIGHTS];
	float distanceLight[MAX_TOTAL_LIGHTS];
	vec4 color;
	vec4 vertWorldSpace;
	vec4 vertViewSpace;
} DataIn;

// Light Structure
struct Light {
	int m_type;
	float m_power;
	float m_shine;
	float m_reserved;

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

layout(std140) uniform ub_material {
    Material material;
};

/*
Material g_mat = {
	100.0f,	// shine
	0.0f,
	0.0f,
	0.0f,
	vec4(1.0f, 1.0f, 1.0f, 1.0f),	// ambient
	vec4(1.0f, 1.0f, 1.0f, 1.0f),	// diffuse
	vec4(1.0f, 1.0f, 1.0f, 1.0f)	// specular
};
*/

layout(std140) uniform ub_LightArray {
	Light lights[MAX_TOTAL_LIGHTS];
	int numLights;	
};

layout (location = 0) out vec4 out_vec4Color;

vec4 g_vec4AmbientLightLevel = 0.01 * vec4(1.0, 1.0, 1.0, 0.0);

void CalculateFragmentLightValue(in Light light, in vec4 vertWorldSpace, in vec3 vectorNormal, in vec3 directionLight, in float distanceLight, out float diffuseValue, out float specularValue) {
	//float attenuation = 1 / pow(distanceLight, 2);
	float attenuation = 1.0 / (1.0 + 0.1*distanceLight + 0.01*distanceLight*distanceLight);
	float cosThetaOfLightToVert = max(0.0f, dot(vectorNormal, directionLight));
	diffuseValue = (light.m_power * attenuation) * cosThetaOfLightToVert;
	
	if(diffuseValue > 0.0) {
		vec3 halfVector = normalize(directionLight + normalize(DataIn.directionEye));
		specularValue = pow(max(0.0f, dot(halfVector, vectorNormal.xyz)), material.m_shine) * attenuation;
	}
	else {
		specularValue = 0.0f;
	}
}

void main(void) {  
	
	vec4 vec4LightValue = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	float diffuseValue = 0.0f, specularValue = 0.0f;

	for(int i = 0; i < numLights; i++) {
		CalculateFragmentLightValue(lights[i], DataIn.vertWorldSpace, normalize(DataIn.normal.xyz), normalize(DataIn.directionLight[i]), DataIn.distanceLight[i], diffuseValue, specularValue);

		vec4LightValue += diffuseValue * lights[i].m_colorDiffuse;
		vec4LightValue += specularValue * lights[i].m_colorSpecular;
	}
	vec4LightValue[3] = 1.0f;
	
	out_vec4Color = max((vec4LightValue * DataIn.color), g_vec4AmbientLightLevel);
}