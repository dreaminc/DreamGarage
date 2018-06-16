// blinnPhoneTexTBNBump.vert
// shadertype=glsl

// This shader implements Blinn Phong lighting with color and bump textures
// using a BTN matrix

// Depends: lightingCommon.shader, materialCommon.shader

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
	vec3 vertTBNSpace;
} DataIn;

uniform	bool u_fUseColorTexture;
uniform sampler2D u_textureColor;

uniform	bool u_fUseBumpTexture;
uniform sampler2D u_textureBump;

layout (location = 0) out vec4 out_vec4Color;

float g_ambient = 0.01f;

vec4 g_vec4AmbientLightLevel = g_ambient * material.m_colorAmbient;

void main(void) {  
	
	vec4 vec4LightValue = vec4(0.0f, 0.0f, 0.0f, 1.0f);

	float diffuseValue = 0.0f;
	float specularValue = 0.0f;
	
	vec3 TBNNormal = vec3(0.0f, 0.0f, 1.0f);;
	
	if(u_fUseBumpTexture == true) {
		TBNNormal = texture(u_textureBump, DataIn.uvCoord).rgb;
		TBNNormal = normalize(TBNNormal * 2.0f - 1.0f);   
	}

	vec3 directionEye = normalize(-DataIn.vertTBNSpace);

	for(int i = 0; i < numLights; i++) {
		vec3 directionLight = normalize(DataIn.directionLight[i]);

		if(dot(TBNNormal, directionLight) > 0.0f) {
			CalculateFragmentLightValue(lights[i].m_power, TBNNormal, directionLight, directionEye, DataIn.distanceLight[i], diffuseValue, specularValue);
			
			vec4LightValue += diffuseValue * lights[i].m_colorDiffuse * material.m_colorDiffuse;
			vec4LightValue += specularValue * lights[i].m_colorSpecular * material.m_colorSpecular;
		}
	}
	vec4LightValue[3] = 1.0f;
	
	vec4 textureColor = texture(u_textureColor, DataIn.uvCoord);

	if(u_fUseColorTexture == true) {
		vec4 ambientColor = g_vec4AmbientLightLevel * textureColor;
		out_vec4Color = max((vec4LightValue * DataIn.color * textureColor), ambientColor);
	}
	else {
		vec4 ambientColor = g_vec4AmbientLightLevel;
		out_vec4Color = max((vec4LightValue * DataIn.color), ambientColor);
	}
}