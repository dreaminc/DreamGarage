// irradianceMap.frag
// shadertype=glsl

// Irradiance image based lighing shader

in vec3 inF_vec3Color;

in Data {
    vec4 normal;
    vec3 directionEye;
	vec3 directionLight[MAX_TOTAL_LIGHTS];
	float distanceLight[MAX_TOTAL_LIGHTS];
	vec4 color;
	vec4 vertWorldSpace;
	vec4 vertViewSpace;
	vec4 normalOrig;
} DataIn;

uniform vec4 u_vec4Eye;
uniform mat4 u_mat4Model;
uniform mat4 u_mat4View;

layout (binding = 0) uniform samplerCube u_textureCubeMap;
uniform bool u_hasTextureCubemap;

layout (location = 0) out vec4 out_vec4Color;

mat4 g_mat4ModelView = u_mat4View * u_mat4Model;
mat4 g_mat4InvTransposeModelView = transpose(inverse(g_mat4ModelView));

void main(void) {  
	
	vec4 vec4LightValue = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	float diffuseValue = 0.0f;
	float specularValue = 0.0f;
	
	vec3 normal = normalize(DataIn.normal.xyz);
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

	// Irradiance Mapping
	if(u_hasTextureCubemap == true) {
		ambientColor = ambientIBL(u_textureCubeMap, u_mat4Model, u_vec4Eye, DataIn.vertWorldSpace, DataIn.normalOrig, true);
	}
	
	//out_vec4Color = vec4LightValue;
	//out_vec4Color = (vec4LightValue * DataIn.color) + ambientColor;

	out_vec4Color = max((vec4LightValue * DataIn.color), ambientColor);
}