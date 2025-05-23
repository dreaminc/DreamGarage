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

		vLightDirection = normalize(mat3(mat4View) * -light.m_vectorDirection.xyz);
		//vLightDirection = vec3(normalize(mat4View * vec4(-light.m_vectorDirection.xyz, 0.0f)));
		//vLightDirection = normalize(vec3(-light.m_vectorDirection));
		//vLightDirection = normalize(vec3(mat3(mat4View) * (-light.m_vectorDirection.xyz)));

		lightDistance = 0.0f;
	}
	else  {
		// Point Light
		//vec3 ptLightViewSpace = vec3(mat4View * vec4(light.m_ptOrigin.xyz, 1.0f));
		//vLightDirection = normalize(ptLightViewSpace.xyz - vertViewSpace.xyz);
		
		vLightDirection = normalize(light.m_ptOrigin.xyz - vertWorldSpace.xyz);
		lightDistance = length(light.m_ptOrigin.xyz - vertWorldSpace.xyz);
	}
}

void CalculateFragmentLightValue(in float power, in float shine, in vec3 vectorNormal, in vec3 directionLight, in vec3 directionEye, in float distanceLight, out float diffuseValue, out float specularValue) {
	//float attenuation = 1.0f / pow(distanceLight, 2);
	float attenuation = 1.0f / (1.0 + 0.1 * distanceLight + 0.01 * distanceLight * distanceLight);

	float cosThetaOfLightToVert = max(0.0f, dot(vectorNormal, directionLight));
	diffuseValue = (power * attenuation) * cosThetaOfLightToVert;

	if (diffuseValue > 0.0f) {
		vec3 halfVector = normalize(directionLight + directionEye);
		specularValue = pow(max(0.0f, dot(vectorNormal, halfVector)), shine) * attenuation * power;
	}
	else {
		specularValue = 0.0f;
	}
}

void CalculateFragmentLightValueToon(in float power, in float shine, in vec3 vectorNormal, in vec3 directionLight, in vec3 directionEye, in float distanceLight, out float diffuseValue, out float specularValue, out float outlineValue) {
	
	float cosThetaOfLightToVert = max(0.0f, dot(vectorNormal, directionLight));
	float attenuation = 1.0f / (1.0f + 0.1f * distanceLight + 0.01f * distanceLight*distanceLight);

	if (attenuation * max(0.0, dot(vectorNormal, directionLight)) >= 0.15f) {
		diffuseValue = 1.0f;
	}
	else {
		diffuseValue = 0.0f;
	}

	// Light source on the right side?
	if (cosThetaOfLightToVert > 0.0 &&
		attenuation * pow(max(0.0, dot(reflect(-directionLight, vectorNormal), directionEye)), shine) > 0.5)
	{
		specularValue = 1.0f;
	}
	else {
		specularValue = 0.0f;
	}

	float unlitOutlineThickness = 0.4;
	float litOutlineThickness = 0.7;

	// Outline
	if (dot(directionEye, vectorNormal) < mix(unlitOutlineThickness, litOutlineThickness, max(0.0, dot(vectorNormal, directionLight))))
	{
		outlineValue = 1.0f;
	}
	else {
		outlineValue = 0.0f;
	}
}

