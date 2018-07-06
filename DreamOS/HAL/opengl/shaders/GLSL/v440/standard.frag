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

uniform bool	u_hasTextureAmbient;
uniform sampler2D u_textureAmbient;

uniform bool	u_hasTextureDiffuse;
uniform sampler2D u_textureDiffuse;

uniform bool	u_hasTextureColor;
uniform sampler2D u_textureColor;

uniform bool	u_hasTextureSpecular;
uniform sampler2D u_textureSpecular;

uniform bool	u_fRiverAnimation;
uniform bool	u_fAREnabled;

layout (location = 0) out vec4 out_vec4Color;

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

	//if (u_fRiverAnimation) {
	//	colorAmbient = EnableRiverAnimation();	
	//	colorDiffuse = EnableRiverAnimation();	
	//}

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

	// Keeping the alpha value outside max() helps with distance-mapped fonts;
	// max() is component-wise, and some alpha values currently default to one
	
	// opaque/fully transparent blending without reordering
	// EnableBlending(colorAmbient.a, colorDiffuse.a);
	
	float effectiveAmbient = material.m_ambient;
	if (u_fAREnabled) {
		effectiveAmbient += 0.35f;
		effectiveAmbient = clamp(effectiveAmbient, 0.0f, 1.0f);
	}

	//vec4 lightColorAmbient = effectiveAmbient * colorAmbient;
	vec4 lightColorAmbient = effectiveAmbient * colorDiffuse;
	vec4 outColor = max(vec4LightValue, lightColorAmbient);	

	// Increasing the saturation
	if (u_fAREnabled) {
		out_vec4Color = IncreaseColorSaturation(outColor);
	}

	out_vec4Color = outColor;
}
