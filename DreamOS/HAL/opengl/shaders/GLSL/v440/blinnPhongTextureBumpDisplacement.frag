// blinnPhoneTexTBNBump.vert
// shadertype=glsl

// This shader implements Blinn Phong lighting with color and bump textures
// using a BTN matrix

// Depends: lightingCommon.shader, materialCommon.shader

in vec3 inF_vec3Color;

in Data {
    vec4 normal;
	vec4 tangent;
	vec3 directionLight[MAX_TOTAL_LIGHTS];
	float distanceLight[MAX_TOTAL_LIGHTS];
	vec4 color;
	vec2 uvCoord;
	vec4 vertViewSpace;
	//mat3 TangentBitangentNormalMatrix;
} DataIn;

uniform mat4 u_mat4Model;
uniform mat4 u_mat4View;

uniform	bool u_fHasColorTexture;
uniform sampler2D u_textureColor;

uniform	bool u_fHasBumpTexture;
uniform sampler2D u_textureBump;

uniform	bool u_fHasDisplacementTexture;
uniform sampler2D u_textureDisplacement;

layout (location = 0) out vec4 out_vec4Color;

float g_ambient = 0.01f;

vec4 g_vec4AmbientLightLevel = g_ambient * material.m_colorAmbient;

mat4 g_mat4ModelView = u_mat4View * u_mat4Model;
mat4 g_mat4InvTransposeModelView = transpose(inverse(g_mat4ModelView));

void main(void) {  
	
	vec4 vec4LightValue = vec4(0.0f, 0.0f, 0.0f, 1.0f);

	float diffuseValue = 0.0f;
	float specularValue = 0.0f;
	
	vec3 TBNNormal = vec3(0.0f, 0.0f, 1.0f);
	
	mat3 tangentBitangentNormalMatrix = CalculateTBNMatrix(g_mat4InvTransposeModelView, DataIn.tangent, DataIn.normal);	

	if(u_fHasBumpTexture == true) {
		TBNNormal = texture(u_textureBump, DataIn.uvCoord).rgb;
		TBNNormal = normalize((TBNNormal * 2.0f) - 1.0f);   
	}

	vec3 directionEye = tangentBitangentNormalMatrix * normalize(-DataIn.vertViewSpace.xyz);

	for(int i = 0; i < numLights; i++) {
		vec3 directionLight = tangentBitangentNormalMatrix * normalize(DataIn.directionLight[i]);

		if(dot(TBNNormal, directionLight) > 0.0f) {
			CalculateFragmentLightValue(lights[i].m_power, material.m_shine, TBNNormal, directionLight, directionEye, DataIn.distanceLight[i], diffuseValue, specularValue);
			
			vec4LightValue += diffuseValue * lights[i].m_colorDiffuse * material.m_colorDiffuse;
			vec4LightValue += specularValue * lights[i].m_colorSpecular * material.m_colorSpecular;
		}
	}
	vec4LightValue[3] = 1.0f;

	if(u_fHasColorTexture == true) {
		vec4 textureColor = texture(u_textureColor, DataIn.uvCoord);
		vec4 ambientColor = g_vec4AmbientLightLevel * textureColor;
		out_vec4Color = max((vec4LightValue * DataIn.color * textureColor), ambientColor);
	}
	else {
		vec4 ambientColor = g_vec4AmbientLightLevel;
		out_vec4Color = max((vec4LightValue * DataIn.color), ambientColor);
	}
}