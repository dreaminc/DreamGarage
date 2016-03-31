// minimal.vert
// shadertype=glsl

#version 440 core

#define MAX_TOTAL_LIGHTS 10

// TODO: Move to a uniform block
layout (location = 0) in vec4 inV_vec4Position;
layout (location = 1) in vec4 inV_vec4Color;
layout (location = 2) in vec4 inV_vec4Normal;

//out vec3 inF_vec3Color;

out Data {
	vec4 normal;
	vec3 directionEye;
	vec3 directionLight[MAX_TOTAL_LIGHTS];
	float distanceLight[MAX_TOTAL_LIGHTS];
	vec4 color;
	vec4 vertWorldSpace;
	vec4 vertViewSpace;
} DataOut;

uniform vec4 u_vec4Eye;
uniform mat4 u_mat4Model;
uniform mat4 u_mat4View;
uniform mat4 u_mat4ModelView;
uniform mat4 u_mat4ViewProjection;
uniform mat4 u_mat4Normal;

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

Material g_mat = {
	100.0f,	// shine
	0.0f,
	0.0f,
	0.0f,
	vec4(1.0f, 1.0f, 1.0f, 1.0f),	// ambient
	vec4(1.0f, 1.0f, 1.0f, 1.0f),	// diffuse
	vec4(1.0f, 1.0f, 1.0f, 1.0f)	// specular
};

layout(std140) uniform ub_LightArray {
	Light lights[MAX_TOTAL_LIGHTS];
	int numLights;	
};

vec4 g_vec4AmbientLightLevel = 0.05 * vec4(1.0, 1.0, 1.0, 0.0);

// TODO: Move to CPU side
mat4 g_mat4ModelView = u_mat4View * u_mat4Model;
mat4 g_mat4InvTransposeModel = transpose(inverse(g_mat4ModelView));
//mat4 g_mat4InvTransposeModel = transpose(inverse(u_mat4Model));

void CalculateVertexLightValue(in Light light, in vec4 vertWorldSpace, in vec4 vertViewSpace, in vec4 vectorNormal, in vec3 directionLight, out float diffuseValue, out float specularValue) {
	float distanceLight = length(light.m_ptOrigin.xyz - vertWorldSpace.xyz);
	//vec3 directionLight = normalize(light.m_ptOrigin.xyz - vertWorldSpace.xyz);
		
	float cosThetaOfLightToVert = max(0, dot(vectorNormal.xyz, directionLight.xyz));

	diffuseValue = (light.m_power / (distanceLight * distanceLight)) * cosThetaOfLightToVert;
	//diffuseValue = (light.m_power) * cosThetaOfLightToVert;

	if(diffuseValue > 0.0) {
		//vec3 directionEye = normalize(u_vec4Eye.xyz - vertWorldSpace.xyz);
		vec3 halfVector = normalize(directionLight + DataOut.directionEye);

		specularValue = pow(max(dot(halfVector, vectorNormal.xyz), 0.0), g_mat.m_shine);
	}
	else {
		specularValue = 0.0f;
	}
}

void main(void) {	
	vec4 vertWorldSpace = u_mat4Model * vec4(inV_vec4Position.xyz, 1.0f);
	vec4 vertViewSpace = u_mat4View * u_mat4Model * vec4(inV_vec4Position.xyz, 1.0f);

	DataOut.directionEye = -normalize(vertViewSpace.xyz);
	vec4 vec4ModelNormal = g_mat4InvTransposeModel * normalize(vec4(inV_vec4Normal.xyz, 0.0f));
	
	///*
	vec4 vec4LightValue = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	float diffuseValue = 0.0f, specularValue = 0.0f;
	for(int i = 0; i < numLights; i++) {
		vec3 ptLightViewSpace = vec3(u_mat4View * lights[i].m_ptOrigin);
		DataOut.directionLight[i] = normalize(ptLightViewSpace.xyz - vertViewSpace.xyz);
		DataOut.distanceLight[i] = length(lights[i].m_ptOrigin.xyz - vertWorldSpace.xyz);

		/*
		CalculateVertexLightValue(lights[i], vertWorldSpace, vertViewSpace, vec4ModelNormal, DataOut.directionLight[i], diffuseValue, specularValue);
		vec4LightValue += diffuseValue * lights[i].m_colorDiffuse;
		vec4LightValue += specularValue * lights[i].m_colorSpecular;
		*/
	}
	//*/

	// Projected Vert Position
	gl_Position = u_mat4ViewProjection * vertWorldSpace;
	
	DataOut.vertWorldSpace = vertWorldSpace;
	DataOut.vertViewSpace = vertViewSpace;
	DataOut.normal = vec4ModelNormal;

	// Vert Color
	//DataOut.color = (vec4LightValue * inV_vec4Color) + g_vec4AmbientLightLevel;
	DataOut.color = inV_vec4Color;
}