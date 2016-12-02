// blinnPhoneTexTBNBump.vert
// shadertype=glsl

// This shader implements Blinn Phong lighting with color and bump textures
// using a BTN matrix

#version 440 core

#define MAX_TOTAL_LIGHTS 10

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
	float oscillateDisplacement;
} DataIn;

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
	float m_bump;
	float reserved2;
	float reserved3;
    vec4 m_colorAmbient;
    vec4 m_colorDiffuse;
    vec4 m_colorSpecular;
};

layout(std140) uniform ub_material {
    Material material;
};

layout(std140) uniform ub_Lights {
	Light lights[MAX_TOTAL_LIGHTS];
	int numLights;	
};

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

uniform bool	u_fOscillate;

layout (location = 0) out vec4 out_vec4Color;

float g_ambient = 0.03f;

vec4 g_vec4AmbientLightLevel = g_ambient * material.m_colorAmbient;

void CalculateFragmentLightValue(in float power, in vec3 vectorNormal, in vec3 directionLight, in vec3 directionEye, in float distanceLight, out float diffuseValue, out float specularValue) {
	//float attenuation = 1 / pow(distanceLight, 2);
	float attenuation = 1.0 / (1.0 + 0.1*distanceLight + 0.01*distanceLight*distanceLight);
	//float attenuation = 1.0f;

	float cosThetaOfLightToVert = max(0.0f, dot(vectorNormal, directionLight));
	diffuseValue = (power * attenuation) * cosThetaOfLightToVert;

	///*
	if(diffuseValue > 0.0f) {
		//vec3 halfVector = normalize(directionLight + normalize(DataIn.directionEye));
		vec3 halfVector = normalize(directionLight + directionEye);
		specularValue = pow(max(0.0f, dot(halfVector, vectorNormal.xyz)), material.m_shine) * attenuation;
	}
	else {
		specularValue = 0.0f;
	}
	//*/
}

void EnableBlending(float ambientAlpha, float diffuseAlpha) {
	// 
	// Fakes blending by moving clear fragments behind the skybox
	// Remove once blending is fully supported
	if (ambientAlpha < 0.1f || diffuseAlpha < 0.1f) {
		gl_FragDepth = 1.0f;
	} 
	else {
		gl_FragDepth = gl_FragCoord.z;
	}
}
//*

//vec4 lightColor = vec4(57.0f / 255.0f, 158.0f / 255.0f, 253.0f / 255.0f, 1.0f);
//vec4 darkColor  = vec4(24.0f / 255.0f,  77.0f / 255.0f, 174.0f / 255.0f, 1.0f);
vec4 lightColor = vec4(76.0f / 255.0f, 203.0f / 255.0f, 247.0f / 255.0f, 1.0f);
vec4 darkColor  = vec4(21.0f / 255.0f,  50.0f / 255.0f, 115.0f / 255.0f, 1.0f);
vec4 black		= vec4(0.0f, 0.0f, 0.0f, 0.0f);
vec4 EnableOscillation(vec4 colorIn) {
	if (u_fOscillate) {
		float color = ((1.5f * DataIn.oscillateDisplacement) + 1.0f) / 2.0f;

		vec4 guess = (color*lightColor) + ((1.0f - color)*darkColor);

		 return vec4(guess.xyz, 1.0f);
	}
	return colorIn;
}
//*/

void main(void) {  
	
	vec4 vec4LightValue = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	float diffuseValue = 0.0f, specularValue = 0.0f;
	
	vec3 TBNNormal;
	
	if(u_hasBumpTexture == true) {
		TBNNormal = texture(u_textureBump, DataIn.uvCoord).rgb;
		TBNNormal = normalize(TBNNormal * 2.0f - vec3(1.0f));   
	}
	else {
		TBNNormal = vec3(0.0f, 0.0f, 1.0f);
	}

	vec4 colorAmbient = material.m_colorAmbient * ((u_hasTextureAmbient) ? texture(u_textureAmbient, DataIn.uvCoord * 1.0f) : (u_hasTextureColor) ? texture(u_textureColor, DataIn.uvCoord * 1.0f) : vec4(1, 1, 1, 1));
	vec4 colorDiffuse = material.m_colorDiffuse * ((u_hasTextureDiffuse) ? texture(u_textureDiffuse, DataIn.uvCoord * 1.0f) : vec4(1, 1, 1, 1));
	vec4 colorSpecular = material.m_colorSpecular * ((u_hasTextureSpecular) ? texture(u_textureSpecular, DataIn.uvCoord * 1.0f) : vec4(1, 1, 1, 1));

	colorAmbient = EnableOscillation(colorAmbient);	
	colorDiffuse = EnableOscillation(colorDiffuse);	

	vec4 lightColorAmbient = g_ambient * vec4(1,1,1,1);

	vec3 directionEye = normalize(-DataIn.vertTBNSpace);

	for(int i = 0; i < numLights; i++) {
		vec3 directionLight = normalize(DataIn.directionLight[i]);

		vec4LightValue += lightColorAmbient * colorAmbient;

		if(dot(vec3(0.0f, 0.0f, 1.0f), directionLight) > 0.0f) {
			CalculateFragmentLightValue(lights[i].m_power, TBNNormal, directionLight, directionEye, DataIn.distanceLight[i], diffuseValue, specularValue);
			vec4LightValue += diffuseValue * lights[i].m_colorDiffuse * colorDiffuse;
			vec4LightValue += specularValue * lights[i].m_colorSpecular * colorSpecular;
		}
	}

	out_vec4Color = vec4LightValue;

	// opaque/fully transparent blending without reordering
	EnableBlending(colorAmbient.a, colorDiffuse.a);
}