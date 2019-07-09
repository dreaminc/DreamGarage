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

uniform bool	u_hasTextureAmbient;
uniform sampler2D u_textureAmbient;

uniform bool	u_hasTextureDiffuse;
uniform sampler2D u_textureDiffuse;

uniform bool	u_hasTextureSpecular;
uniform sampler2D u_textureSpecular;

layout (location = 0) out vec4 out_vec4Color;

uniform vec4 u_vec4ClippingPlane;

void main(void) {  
	// Clip fragments on our side of the plane
	float fragmentClipPosition = dot(DataIn.vertWorldSpace.xyz, normalize(u_vec4ClippingPlane.xyz)) + u_vec4ClippingPlane.w;
    if (fragmentClipPosition < 0.0) {
		discard;
    }

	vec4 vec4LightValue = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	float diffuseValue = 0.0f;
	float specularValue = 0.0f;
	vec2 uvCoord = DataIn.uvCoord;
	
	// tile the textures
	uvCoord.x *= material.m_tilingU;
	uvCoord.y *= material.m_tilingV;
	uvCoord = mod(uvCoord, 1.0f);

	vec3 TBNNormal = vec3(0.0f, 0.0f, 1.0f);
	
	if(u_hasBumpTexture == true) {
		TBNNormal = texture(u_textureBump, uvCoord).rgb;
		TBNNormal = normalize(TBNNormal * 2.0f - 1.0f); 
		TBNNormal.xy *= material.m_bumpiness; 
		TBNNormal = normalize(TBNNormal);
	}

	// Generalize
	vec4 colorDiffuse = material.m_colorDiffuse;
	if(u_hasTextureDiffuse) {
		colorDiffuse *= texture(u_textureDiffuse, uvCoord);
	}

	vec4 colorAmbient = material.m_colorAmbient; 
	if (u_hasTextureAmbient) {
		colorAmbient *= texture(u_textureAmbient, uvCoord);
	}

	vec4 colorSpecular = material.m_colorSpecular; 
	if(u_hasTextureSpecular) {
		colorSpecular *= texture(u_textureSpecular, uvCoord);
	}

	vec3 directionEye = normalize(-DataIn.vertTBNSpace);

	for(int i = 0; i < numLights; i++) {
		vec3 directionLight = normalize(DataIn.directionLight[i]);
	
		if(dot(TBNNormal, directionLight) > 0.0f) {
			CalculateFragmentLightValue(lights[i].m_power, material.m_shine, TBNNormal, directionLight, directionEye, DataIn.distanceLight[i], diffuseValue, specularValue);
			
			vec4LightValue += diffuseValue * lights[i].m_colorDiffuse * colorDiffuse;
			vec4LightValue += specularValue * lights[i].m_colorSpecular * colorSpecular;
		}
	}
	vec4LightValue[3] = 1.0f;

	vec4 lightColorAmbient = material.m_ambient * colorAmbient;
	vec4 outColor = max(vec4LightValue, lightColorAmbient);	
	
	out_vec4Color = outColor;
}
