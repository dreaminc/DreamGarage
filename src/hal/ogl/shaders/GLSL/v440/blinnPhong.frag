// blinnPhong.vert
// shadertype=glsl

// This is a basic blinn phong shader 

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

layout (location = 0) out vec4 out_vec4Color;

void main(void) {  
	
	vec4 vec4LightValue = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	float diffuseValue = 0.0f;
	float specularValue = 0.0f;
	
	vec3 normal = normalize(DataIn.normal.xyz);
	//vec3 directionEye = normalize(DataIn.directionEye);
	vec3 directionEye = -normalize(DataIn.vertViewSpace.xyz);

	for(int i = 0; i < numLights; i++) {
		vec3 directionLight = normalize(DataIn.directionLight[i]);

		if(dot(normal, directionLight) > 0.0f) {
			CalculateFragmentLightValue(lights[i].m_power, material.m_shine, normal, directionLight, directionEye, DataIn.distanceLight[i], diffuseValue, specularValue);
			//CalculateFragmentLightValue(lights[i].m_power, 60.0f, normal, directionLight, directionEye, DataIn.distanceLight[i], diffuseValue, specularValue);
			
			vec4LightValue += diffuseValue * lights[i].m_colorDiffuse * material.m_colorDiffuse;
			vec4LightValue += specularValue * lights[i].m_colorSpecular * material.m_colorSpecular;
		}
	}
	vec4LightValue[3] = 1.0f;
	
	vec4 ambientColor = material.m_ambient * material.m_colorAmbient;
	out_vec4Color = max((vec4LightValue * DataIn.color), ambientColor);
}