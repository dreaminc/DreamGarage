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
	vec2 uvCoord;
	vec4 vertWorldSpace;
	vec4 vertViewSpace;
} DataIn;

uniform	bool u_fUseColorTexture;
uniform sampler2D u_textureColor;

layout (location = 0) out vec4 out_vec4Color;

float g_ambient = 0.1f;

vec4 g_vec4AmbientLightLevel = g_ambient * material.m_colorAmbient;

void main(void) {  
	
	vec4 vec4LightValue = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	float diffuseValue = 0.0f, specularValue = 0.0f;
	float outlineValue = 0.0f;
	vec4 outlineColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	
	vec3 normal = normalize(DataIn.normal.xyz);
	//vec3 directionEye = normalize(DataIn.directionEye);
	vec3 directionEye = -normalize(DataIn.vertViewSpace.xyz);

	for(int i = 0; i < numLights; i++) {
		vec3 directionLight = normalize(DataIn.directionLight[i]);

		if(dot(normal, directionLight) > 0.0f) {
			CalculateFragmentLightValueToon(lights[i].m_power, material.m_shine, normal, directionLight, directionEye, DataIn.distanceLight[i], diffuseValue, specularValue, outlineValue);
			
			vec4LightValue += diffuseValue * lights[i].m_colorDiffuse * material.m_colorDiffuse;
			vec4LightValue += specularValue * lights[i].m_colorSpecular * material.m_colorSpecular;
		}
	}
	vec4LightValue[3] = 1.0f;
	
	vec4 textureColor = texture(u_textureColor, DataIn.uvCoord * 1.0f);

	vec4LightValue += g_ambient * vec4(1.0f, 1.0f, 1.0f, 1.0f);

	if(outlineValue == 0.0f) {
		//vec4 ambientColor = g_vec4AmbientLightLevel;		
		//out_vec4Color = max((vec4LightValue * (DataIn.color + textureColor)), ambientColor);
		//out_vec4Color = vec4LightValue * (DataIn.color + textureColor);
		out_vec4Color = vec4LightValue * textureColor;
	}
	else {
		out_vec4Color = outlineColor;
	}
}