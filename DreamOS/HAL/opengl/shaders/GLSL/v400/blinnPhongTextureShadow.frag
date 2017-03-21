// minimal.vert
// shadertype=glsl

// This is a minimal shader that simply displays
// a vertex color and position (no lighting, no textures)

#version 400 core

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
	
	vec4 vertShadowCoordinate;
	vec3 directionShadowCastingLight;
} DataIn;

uniform bool	u_hasTextureColor;
uniform sampler2D u_textureColor;

uniform bool	u_hasTextureDepth;
uniform sampler2D u_textureDepth;

uniform bool	u_hasTextureAmbient;
uniform sampler2D u_textureAmbient;

uniform bool	u_hasTextureDiffuse;
uniform sampler2D u_textureDiffuse;

uniform bool	u_hasTextureSpecular;
uniform sampler2D u_textureSpecular;

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
	float m_ambient;
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

layout (location = 0) out vec4 out_vec4Color;

float g_ambient = 0.01f;

vec4 g_vec4AmbientLightLevel = g_ambient * material.m_colorAmbient;

void CalculateFragmentLightValue(in float power, in vec3 vectorNormal, in vec3 directionLight, in float distanceLight, out float diffuseValue, out float specularValue) {
	//float attenuation = 1 / pow(distanceLight, 2);
	float attenuation = 1.0 / (1.0 + 0.1*distanceLight + 0.01*distanceLight*distanceLight);
	//float attenuation = 1.0f;

	float cosThetaOfLightToVert = max(0.0f, dot(vectorNormal, directionLight));
	diffuseValue = (power * attenuation) * cosThetaOfLightToVert;
	
	///*
	if(diffuseValue > 0.0f) {
		vec3 halfVector = normalize(directionLight + normalize(DataIn.directionEye));
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

void main(void) {  
	
	vec4 vec4LightValue = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	float diffuseValue = 0.0f, specularValue = 0.0f;

	vec3 normal = normalize(DataIn.normal.xyz);
	//vec3 directionEye = normalize(DataIn.directionEye);
	vec3 directionEye = -normalize(DataIn.vertViewSpace.xyz);

	// TODO: This is a hack, currently hard coded values
	float lightVisibility = 1.0f;
	float cosTheta = dot(normal, DataIn.directionShadowCastingLight);
	float bias = 0.005f * tan(acos(cosTheta)); // cosTheta is dot( n,l ), clamped between 0 and 1
	bias = clamp(bias, 0.0f, 0.01f);
	bias = 0.0f;

	///*
	if (u_hasTextureDepth) {
		if(texture(u_textureDepth, DataIn.vertShadowCoordinate.xy).x  <  (DataIn.vertShadowCoordinate.z - bias)) {
			lightVisibility = 0.5;
		}
	}
	//*/

	/*
	for (int i=0; i < 4; i++){
		if(texture(u_textureDepth, DataIn.vertShadowCoordinate.xy + poissonDisk[i]/700.0).x  <  (DataIn.vertShadowCoordinate.z - bias)){
			//lightVisibility = 0.5;
			lightVisibility -= 0.2;
		}
	}
	//*/
	
	vec3 TBNNormal = vec3(0.0f, 0.0f, 1.0f);
	//TBNNormal = normalize(DataIn.TangentBitangentNormalMatrix * TBNNormal);

	vec4 colorAmbient = material.m_colorAmbient * ((u_hasTextureAmbient) ? texture(u_textureAmbient, DataIn.uvCoord * 1.0f) : (u_hasTextureColor) ? texture(u_textureColor, DataIn.uvCoord * 1.0f) : vec4(1, 1, 1, 1));
	vec4 colorDiffuse = material.m_colorDiffuse * ((u_hasTextureDiffuse) ? texture(u_textureDiffuse, DataIn.uvCoord * 1.0f) : vec4(1, 1, 1, 1));
	vec4 colorSpecular = material.m_colorSpecular * ((u_hasTextureSpecular) ? texture(u_textureSpecular, DataIn.uvCoord * 1.0f) : vec4(1, 1, 1, 1));
	
	
	// TODO: light.m_colorAmbient now hardcoded, needs to be part of the light property
	vec4 lightColorAmbient = g_ambient * vec4(1,1,1,1);

	vec4 ambientValue = vec4(1,1,1,1);

	for(int i = 0; i < numLights; i++) {
		vec3 directionLight = normalize(DataIn.directionLight[i]);
		
		vec4LightValue += lightVisibility * ambientValue * lightColorAmbient /*lights[i].m_colorDiffuse*/ * colorAmbient;

		if(dot(vec3(0.0f, 0.0f, 1.0f), directionLight) > 0.0f) {
			CalculateFragmentLightValue(lights[i].m_power, TBNNormal, directionLight, DataIn.distanceLight[i], diffuseValue, specularValue);
			vec4LightValue += lightVisibility * diffuseValue * lights[i].m_colorDiffuse * colorDiffuse;
			vec4LightValue += lightVisibility * specularValue * lights[i].m_colorSpecular * colorSpecular;
		}
	}

	out_vec4Color = vec4LightValue;

	// opaque/fully transparent blending without reordering
	EnableBlending(colorAmbient.a, colorDiffuse.a);
}
