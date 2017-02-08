// minimal.vert
// shadertype=glsl

// This is a minimal shader that simply displays
// a vertex color and position (no lighting, no textures)

#version 400 core

#define MAX_TOTAL_LIGHTS 10

in vec3 inF_vec3Color;

in Data {
    vec4 normal;
    vec3 directionEye;
	vec3 directionLight[MAX_TOTAL_LIGHTS];
	float distanceLight[MAX_TOTAL_LIGHTS];
	vec4 color;
	vec2 uvCoord;
	vec4 vertWorldSpace;
	vec4 vertViewSpace;
	mat3 TangentBitangentNormalMatrix;
} DataIn;

uniform	bool u_fUseColorTexture;
uniform sampler2D u_textureColor;

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

layout(std140) uniform ub_Lights {
	Light lights[MAX_TOTAL_LIGHTS];
	int numLights;	
};

layout (location = 0) out vec4 out_vec4Color;

float g_ambient = 0.01f;

vec4 g_vec4AmbientLightLevel = g_ambient * material.m_colorAmbient;

void CalculateFragmentLightValue(in float power, in vec3 vectorNormal, in vec3 directionLight, in float distanceLight, out float diffuseValue, out float specularValue) {
	//float attenuation = 1 / pow(distanceLight, 2);
	float attenuation = 1.0 / (1.0 + 0.1*distanceLight + 0.01*distanceLight*distanceLight);
	//float attenuation = 1.0f;

	float cosThetaOfLightToVert = max(0.0f, dot(vectorNormal, directionLight));
	diffuseValue = (power * attenuation) * cosThetaOfLightToVert;
	
	///*
	if(diffuseValue > 0.0f) {
		vec3 halfVector = normalize(directionLight + normalize(DataIn.directionEye));
		specularValue = pow(max(0.0f, dot(halfVector, vectorNormal.xyz)), material.m_shine) * attenuation;
	}
	else {
		specularValue = 0.0f;
	}
	//*/
}

void main(void) {  
	
	vec4 vec4LightValue = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	float diffuseValue = 0.0f, specularValue = 0.0f;
	
	vec3 TBNNormal = vec3(0.0f, 0.0f, 1.0f);
	//TBNNormal = normalize(DataIn.TangentBitangentNormalMatrix * TBNNormal);

	for(int i = 0; i < numLights; i++) {
		vec3 directionLight = normalize(DataIn.directionLight[i]);

		if(dot(vec3(0.0f, 0.0f, 1.0f), directionLight) > 0.0f) {
			CalculateFragmentLightValue(lights[i].m_power, TBNNormal, directionLight, DataIn.distanceLight[i], diffuseValue, specularValue);
			vec4LightValue += diffuseValue * lights[i].m_colorDiffuse * material.m_colorDiffuse;
			vec4LightValue += specularValue * lights[i].m_colorSpecular * material.m_colorSpecular;
		}
	}
	vec4LightValue[3] = 1.0f;
	
	vec4 textureColor = texture(u_textureColor, DataIn.uvCoord * 1.0f);
	vec4 ambientColor = g_vec4AmbientLightLevel * textureColor;
	if(u_fUseColorTexture == true) {
		out_vec4Color = max((vec4LightValue * DataIn.color * textureColor), ambientColor);
	}
	else {
		out_vec4Color = max((vec4LightValue * DataIn.color), ambientColor);
	}


	//out_vec4Color = textureColor;
}