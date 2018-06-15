// lightingCommon.shader
// shadertype=glsl

// A shared GLSL library for vertex shader stage lighting functionality
// DEPENDS: materialCommon.shader

#define MAX_TOTAL_LIGHTS 10

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

// Input layout for light block
layout(std140) uniform ub_Lights {
	Light lights[MAX_TOTAL_LIGHTS];
	int numLights;	
};

void ProcessLightVertex(in Light light, in mat4 mat4View, in vec4 vertViewSpace, in vec4 vertWorldSpace, out vec3 vLightDirection, out float lightDistance) {
	if(light.m_type == 0) {
		// Directional Light
		vLightDirection = normalize(vec3(mat3(mat4View) * (-light.m_vectorDirection.xyz)));
		lightDistance = 0.0f;
	}
	else  {
		// Point Light
		vec3 ptLightViewSpace = vec3(mat4View * vec4(light.m_ptOrigin.xyz, 1.0f));
		
		vLightDirection = normalize(ptLightViewSpace.xyz - vertViewSpace.xyz);
		lightDistance = length(light.m_ptOrigin.xyz - vertWorldSpace.xyz);
	}
}

void CalculateFragmentLightValue(in float power, in vec3 vectorNormal, in vec3 directionLight, in vec3 directionEye, in float distanceLight, out float diffuseValue, out float specularValue) {
	//float attenuation = 1 / pow(distanceLight, 2);
	float attenuation = 1.0 / (1.0 + 0.1*distanceLight + 0.01*distanceLight*distanceLight);
	//float attenuation = 1.0f;

	float cosThetaOfLightToVert = max(0.0f, dot(vectorNormal, directionLight));
	diffuseValue = (power * attenuation) * cosThetaOfLightToVert;

	///*
	if (diffuseValue > 0.0f) {
		//vec3 halfVector = normalize(directionLight + normalize(DataIn.directionEye));
		vec3 halfVector = normalize(directionLight + directionEye);
		specularValue = pow(max(0.0f, dot(vectorNormal, halfVector)), material.m_shine) * attenuation;
	}
	else {
		specularValue = 0.0f;
	}
	//*/
}
