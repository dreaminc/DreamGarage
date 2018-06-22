// standard.vert
// shadertype=glsl

// This is the standard Dream shader model
// and will include light, material, and texturing capabilities

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

uniform bool	u_fRiverAnimation;
uniform bool	u_fAREnabled;

layout (location = 0) out vec4 out_vec4Color;

float g_ambient = material.m_ambient;

vec4 g_vec4AmbientLightLevel = g_ambient * material.m_colorAmbient;

void EnableBlending(float ambientAlpha, float diffuseAlpha) {
	// Fakes blending by moving clear fragments behind the skybox
	// Remove once blending is fully supported
	if (ambientAlpha < 0.1f || diffuseAlpha < 0.1f) {
		gl_FragDepth = 1.0f;
	} 
	else {
		gl_FragDepth = gl_FragCoord.z;
	}
}

//vec4 lightColor = vec4(57.0f / 255.0f, 158.0f / 255.0f, 253.0f / 255.0f, 1.0f);
//vec4 darkColor  = vec4(24.0f / 255.0f,  77.0f / 255.0f, 174.0f / 255.0f, 1.0f);
vec4 lightColor = vec4(76.0f / 255.0f, 203.0f / 255.0f, 247.0f / 255.0f, 1.0f);
vec4 darkColor  = vec4(21.0f / 255.0f,  50.0f / 255.0f, 115.0f / 255.0f, 1.0f);
//vec4 black		= vec4(0.0f, 0.0f, 0.0f, 0.0f);

vec4 EnableRiverAnimation() {
	float color = ((1.5f * DataIn.riverAnimationDisplacement) + 1.0f) / 2.0f;
	vec4 guess = (color * lightColor) + ((1.0f - color) * darkColor);
	return vec4(guess.xyz, 1.0f);
}
//*/

vec4 IncreaseColorSaturation(vec4 color) {
	//vec4 colorBrighter = color*2;
	//colorBrighter = clamp(colorBrighter, 0.0, 1.0);
	vec4 colorBrighter = vec4(tanh(2*color.r), tanh(2*color.g), tanh(2*color.b), color.a);
	return colorBrighter;	
}

void main(void) {  

	if (u_fAREnabled) {
		g_ambient += 0.35f;
	}
	
	vec4 vec4LightValue = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	float diffuseValue = 0.0f;
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

	if (u_fRiverAnimation) {
		colorAmbient = EnableRiverAnimation();	
		colorDiffuse = EnableRiverAnimation();	
	}

	vec4 lightColorAmbient = g_ambient * vec4(1.0f, 1.0f, 1.0f, 1.0f);

	vec3 directionEye = normalize(-DataIn.vertTBNSpace);

	for(int i = 0; i < numLights; i++) {
		vec3 directionLight = normalize(DataIn.directionLight[i]);

		if(dot(TBNNormal, directionLight) > 0.0f) {
			CalculateFragmentLightValue(lights[i].m_power, TBNNormal, directionLight, directionEye, DataIn.distanceLight[i], diffuseValue, specularValue);
			
			vec4LightValue += diffuseValue * lights[i].m_colorDiffuse * colorDiffuse;
			vec4LightValue += specularValue * lights[i].m_colorSpecular * colorSpecular;
		}
	}

	//out_vec4Color = vec4LightValue;

	// keeping the alpha value outside max() helps with distance-mapped fonts;
	// max() is component-wise, and some alpha values currently default to one
	
	// opaque/fully transparent blending without reordering
	EnableBlending(colorAmbient.a, colorDiffuse.a);
	vec4 outColor = vec4(max(vec4LightValue.xyz, (lightColorAmbient * colorAmbient).xyz), colorDiffuse.a);

	// testing increasing the saturation
	if (u_fAREnabled) {
		out_vec4Color = IncreaseColorSaturation(outColor);
	}

	out_vec4Color = outColor;
}
