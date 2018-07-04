// standard_clipping.frag
// shadertype=glsl

// This is a shader based on the standard shader that includes a clipping
// plane as would be used for reflection or refraction mapping

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
	vec4 vertEyeSpace;
	mat3 TangentBitangentNormalMatrix;
	vec3 vertTBNSpace;
	float riverAnimationDisplacement;
} DataIn;

uniform	bool u_hasBumpTexture;
uniform sampler2D u_textureBump;

uniform bool	u_hasTextureColor;
uniform sampler2D u_textureColor;

uniform bool	u_hasTextureAmbient;
uniform sampler2D u_textureAmbient;

uniform bool	u_hasTextureDiffuse;
uniform sampler2D u_textureDiffuse;

uniform bool	u_hasTextureSpecular;
uniform sampler2D u_textureSpecular;

layout (location = 0) out vec4 out_vec4Color;

float g_ambient = material.m_ambient;

vec4 g_vec4AmbientLightLevel = g_ambient * material.m_colorAmbient;

uniform vec4 u_vec4ClippingPlane;

void main(void) {  
	
	// Clip fragments on our side of the plane
	float fragmentClipPosition = dot(DataIn.vertWorldSpace.xyz, normalize(u_vec4ClippingPlane.xyz)) + u_vec4ClippingPlane.w;
    if (fragmentClipPosition < 0.0) {
		//out_vec4Color = material.m_colorDiffuse;
        //return;
		discard;
    }

	//vec4 vec4LightValue = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	vec4 vec4LightValue = vec4(1.0f);
	float diffuseValue = 1.0f;
	float specularValue = 0.0f;
	
	vec3 TBNNormal = vec3(0.0f, 0.0f, 1.0f);
	
	if(u_hasBumpTexture == true) {
		TBNNormal = texture(u_textureBump, DataIn.uvCoord).rgb;
		TBNNormal = normalize(TBNNormal * 2.0f - 1.0f); 
	}

	// TODO: move this logic outside of the shader
	vec4 colorAmbient = material.m_colorAmbient * ((u_hasTextureAmbient) ? texture(u_textureAmbient, DataIn.uvCoord * 1.0f) : (u_hasTextureColor) ? texture(u_textureColor, DataIn.uvCoord * 1.0f) : vec4(1, 1, 1, 1));
	vec4 colorDiffuse = material.m_colorDiffuse * ((u_hasTextureDiffuse) ? texture(u_textureDiffuse, DataIn.uvCoord * 1.0f) : (u_hasTextureColor) ? texture(u_textureColor, DataIn.uvCoord * 1.0f) : vec4(1, 1, 1, 1));
	vec4 colorSpecular = material.m_colorSpecular * ((u_hasTextureSpecular) ? texture(u_textureSpecular, DataIn.uvCoord * 1.0f) : vec4(1, 1, 1, 1));

	vec4 lightColorAmbient = g_ambient * vec4(1.0f, 1.0f, 1.0f, 1.0f);

	vec3 directionEye = normalize(-DataIn.vertTBNSpace);

	vec4LightValue = colorDiffuse;

	for(int i = 0; i < numLights; i++) {
		vec3 directionLight = normalize(DataIn.directionLight[i]);
	
		if(dot(TBNNormal, directionLight) > 0.0f) {
			CalculateFragmentLightValue(lights[i].m_power, TBNNormal, directionLight, directionEye, DataIn.distanceLight[i], diffuseValue, specularValue);
			
			vec4LightValue += diffuseValue * lights[i].m_colorDiffuse * colorDiffuse;
			vec4LightValue += specularValue * lights[i].m_colorSpecular * colorSpecular;
		}
	}

	out_vec4Color = vec4(max(vec4LightValue.xyz, (lightColorAmbient * colorAmbient).xyz), colorDiffuse.a);
	//out_vec4Color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}
