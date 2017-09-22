// blinnPhongShadow.vert
// shadertype=glsl

// Employs the blinn phong shading algorithm and also utilizes shadows if
// they are set 

#version 440 core

#define MAX_TOTAL_LIGHTS 10
#define SHADOW_PCF_OFFSET 10.0f
//#define _USE_PCF

in vec3 inF_vec3Color;

in Data {
    vec4 normal;
    vec3 directionEye;
	vec3 directionLight[MAX_TOTAL_LIGHTS];
	float distanceLight[MAX_TOTAL_LIGHTS];
	vec4 color;
	vec4 vertWorldSpace;
	vec4 vertViewSpace;

	vec4 vertShadowCoordinate;
	vec3 shadowEmitterDirection;
} DataIn;

uniform sampler2DShadow u_textureDepth;
//uniform sampler2D u_textureDepth;

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

vec2 samplingLookup[] = vec2[](
  vec2( -1.0f, -1.0f ),
  vec2( -1.0f, 1.0 ),
  vec2( 1.0f, -1.0f ),
  vec2( 1.0f, 1.0f )
);

vec4 g_vec4AmbientLightLevel = g_ambient * material.m_colorAmbient;

void CalculateFragmentLightValue(in float power, in vec3 vectorNormal, in vec3 directionLight, in vec3 directionEye, in float distanceLight, out float diffuseValue, out float specularValue) {
	//float attenuation = 1 / pow(distanceLight, 2);
	float attenuation = 1.0 / (1.0 + 0.1*distanceLight + 0.01*distanceLight*distanceLight);
	//float attenuation = 1.0f;

	float cosThetaOfLightToVert = max(0.0f, dot(vectorNormal, directionLight));
	diffuseValue = (power * attenuation) * cosThetaOfLightToVert;
	
	///*
	if(diffuseValue > 0.0f) {
		vec3 halfVector = normalize(directionLight + directionEye);
		specularValue = pow(max(0.0f, dot(vectorNormal, halfVector)), material.m_shine) * attenuation;
	}
	else {
		specularValue = 0.0f;
	}
	//*/
}

void main(void) {  
	
	vec4 vec4LightValue = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	float diffuseValue = 0.0f, specularValue = 0.0f;
	
	vec3 normal = normalize(DataIn.normal.xyz);
	vec3 directionEye = -normalize(DataIn.vertViewSpace.xyz);

	// TODO: This is a hack, currently hard coded values
	float lightVisibility = 1.0f;
	float cosTheta = dot(normal, normalize(DataIn.shadowEmitterDirection.xyz));
	
	float bias = 0.005f * tan(acos(cosTheta)); // cosTheta is dot( n,l ), clamped between 0 and 1
	bias = clamp(bias, 0.0f, 0.01f);

	if(cosTheta > bias) {
	#ifdef 	_USE_PCF
		float shadowAccumulator = 0.0f;

		// weight center more
		shadowAccumulator += 2.0f * textureProjOffset(u_textureDepth, DataIn.vertShadowCoordinate, ivec2(0, 0));

		shadowAccumulator += 1.0f * textureProjOffset(u_textureDepth, DataIn.vertShadowCoordinate, ivec2(-SHADOW_PCF_OFFSET, -SHADOW_PCF_OFFSET));
		shadowAccumulator += 1.0f * textureProjOffset(u_textureDepth, DataIn.vertShadowCoordinate, ivec2(-SHADOW_PCF_OFFSET, 0));
		shadowAccumulator += 1.0f * textureProjOffset(u_textureDepth, DataIn.vertShadowCoordinate, ivec2(-SHADOW_PCF_OFFSET, SHADOW_PCF_OFFSET));

		shadowAccumulator += 1.0f * textureProjOffset(u_textureDepth, DataIn.vertShadowCoordinate, ivec2(0, SHADOW_PCF_OFFSET));
		shadowAccumulator += 1.0f * textureProjOffset(u_textureDepth, DataIn.vertShadowCoordinate, ivec2(0, -SHADOW_PCF_OFFSET));

		shadowAccumulator += 1.0f * textureProjOffset(u_textureDepth, DataIn.vertShadowCoordinate, ivec2(SHADOW_PCF_OFFSET, -SHADOW_PCF_OFFSET));
		shadowAccumulator += 1.0f * textureProjOffset(u_textureDepth, DataIn.vertShadowCoordinate, ivec2(SHADOW_PCF_OFFSET, 0));
		shadowAccumulator += 1.0f * textureProjOffset(u_textureDepth, DataIn.vertShadowCoordinate, ivec2(SHADOW_PCF_OFFSET, SHADOW_PCF_OFFSET));

		
	
		lightVisibility *= (shadowAccumulator * (1.0f/10.0f));
		lightVisibility = clamp(lightVisibility, 0.25f, 1.0f);
	#else
		float shadowVal = textureProj(u_textureDepth, DataIn.vertShadowCoordinate);
		lightVisibility *= shadowVal;
	#endif
	}
	else {
		lightVisibility = 0.0f;
	}

	for(int i = 0; i < numLights; i++) {
		vec3 directionLight = normalize(DataIn.directionLight[i]);

		if(dot(normal, directionLight) > 0.0f) {
			CalculateFragmentLightValue(lights[i].m_power, normal, directionLight, directionEye, DataIn.distanceLight[i], diffuseValue, specularValue);
			
			vec4LightValue += lightVisibility * diffuseValue * lights[i].m_colorDiffuse * material.m_colorDiffuse;
			vec4LightValue += lightVisibility * specularValue * lights[i].m_colorSpecular * material.m_colorSpecular;
		}
	}
	vec4LightValue[3] = 1.0f;
	
	vec4 ambientColor = g_vec4AmbientLightLevel;
	// TODO: This is only good for one light
	out_vec4Color = max((vec4LightValue * DataIn.color), ambientColor);
}