// water.vert
// shadertype=glsl

// A water shader 

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

uniform	bool u_hasTextureReflection;
uniform sampler2D u_textureReflection;

uniform bool	u_hasTextureRefraction;
uniform sampler2D u_textureRefraction;

//uniform bool	u_hasTextureAmbient;
//uniform sampler2D u_textureAmbient;
//
//uniform bool	u_hasTextureDiffuse;
//uniform sampler2D u_textureDiffuse;
//
//uniform bool	u_hasTextureSpecular;
//uniform sampler2D u_textureSpecular;

layout (location = 0) out vec4 out_vec4Color;

uniform vec4 u_vec4ReflectionPlane;

uniform mat4 u_mat4Model;
uniform mat4 u_mat4View;
uniform mat4 u_mat4Projection;
uniform mat4 u_mat4Reflection;

// TODO: Move to CPU side
mat4 xzFlipMatrix = mat4(1.0f, 0.0f, 0.0f, 0.0f,
						 0.0f, -1.0f, 0.0f, 0.0f,
						 0.0f, 0.0f, 1.0f, 0.0f,
						 0.0f, 0.0f, 0.0f, 1.0f);

void main(void) {  
	
	vec4 vec4LightValue = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	float diffuseValue = 0.0f;
	float specularValue = 0.0f;
	
	vec3 TBNNormal = vec3(0.0f, 0.0f, 1.0f);
	
	//if(u_hasBumpTexture == true) {
	//	TBNNormal = texture(u_textureBump, DataIn.uvCoord).rgb;
	//	TBNNormal = normalize(TBNNormal * 2.0f - 1.0f); 
	//}
	
	mat4 mat4ReflectedView = u_mat4Reflection * u_mat4View * xzFlipMatrix;	// This could easily be done on the CPU side
	mat4ReflectedView = mat4ReflectedView - mat4(1.0f);

	mat4ReflectedView = xzFlipMatrix * u_mat4View * (u_mat4Reflection - mat4(1.0f));
	//mat4ReflectedView = u_mat4View * (u_mat4Reflection - mat4(1.0f));
	//mat4ReflectedView =  u_mat4View * u_mat4Reflection * xzFlipMatrix;

	//mat4ReflectedView = u_mat4Reflection * u_mat4View * xzFlipMatrix;
	//mat4ReflectedView = inverse(mat4ReflectedView);

	mat4ReflectedView =  xzFlipMatrix  * (u_mat4Reflection - mat4(1.0f)) * u_mat4View;

	vec4 vClipReflection = u_mat4Projection * mat4ReflectedView * DataIn.vertWorldSpace;
	
	//vec2 vDeviceReflection = vClipReflection.st / vClipReflection.q;
	vec2 vDeviceReflection = vClipReflection.xy / vClipReflection.w;
	vec2 vTextureReflection = vec2(0.5f, 0.5f) + 0.5f * vDeviceReflection;

	//vec4 reflectionTextureColor = texture2D (reflection_sampler, vTextureReflection);

	vec4 colorDiffuse = material.m_colorDiffuse; 
	if(u_hasTextureReflection) {
		//colorDiffuse = colorDiffuse * texture(u_textureReflection, DataIn.uvCoord * 1.0f);
		colorDiffuse = colorDiffuse * texture(u_textureReflection, vTextureReflection);
	}

	vec4 colorAmbient = material.m_ambient * material.m_colorAmbient;

	vec3 directionEye = normalize(-DataIn.vertTBNSpace);

	for(int i = 0; i < numLights; i++) {
		vec3 directionLight = normalize(DataIn.directionLight[i]);

		if(dot(TBNNormal, directionLight) > 0.0f) {
			CalculateFragmentLightValue(lights[i].m_power, TBNNormal, directionLight, directionEye, DataIn.distanceLight[i], diffuseValue, specularValue);
			
			vec4LightValue += diffuseValue * lights[i].m_colorDiffuse * colorDiffuse;
			vec4LightValue += specularValue * lights[i].m_colorSpecular * material.m_colorSpecular;
		}
	}

	out_vec4Color = vec4(max(vec4LightValue.xyz, colorAmbient.xyz), colorDiffuse.a) + vec4(0.2f, 0.2f, 0.2f, 1.0f);
}
