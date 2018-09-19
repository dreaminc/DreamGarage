// standard.vert
// shadertype=glsl

// This is the standard Dream shader model
// and will include light, material, and texturing capabilities

in vec3 inF_vec3Color;

in Data {
    vec4 normal;
	vec4 tangent;
    vec3 directionEye;
	vec3 directionLight[MAX_TOTAL_LIGHTS];
	float distanceLight[MAX_TOTAL_LIGHTS];
	vec4 color;
	vec2 uvCoord;
	vec4 vertWorldSpace;
	vec4 vertViewSpace;
	mat3 TangentBitangentNormalMatrix;
	vec3 vertTBNSpace;
	vec4 normalOrig;
} DataIn;

uniform vec4 u_vec4Eye;
uniform mat4 u_mat4Model;
uniform mat4 u_mat4View;

uniform bool u_hasBumpTexture;
layout (binding = 0) uniform sampler2D u_textureBump;

uniform bool u_hasTextureAmbient;
layout (binding = 1) uniform sampler2D u_textureAmbient;

uniform bool u_hasTextureDiffuse;
layout (binding = 2) uniform sampler2D u_textureDiffuse;

uniform bool u_hasTextureColor;
layout (binding = 3) uniform sampler2D u_textureColor;

uniform bool u_hasTextureSpecular;
layout (binding = 4) uniform sampler2D u_textureSpecular;

uniform bool u_hasCubemapEnvironment;
layout (binding = 5) uniform samplerCube u_cubemapEnvironment;

uniform bool u_hasCubemapIrradiance;
layout (binding = 6) uniform samplerCube u_cubemapIrradiance;

uniform bool	u_fAREnabled;

layout (location = 0) out vec4 out_vec4Color;

// TODO: Move to CPU side
mat4 g_mat4ModelView = u_mat4View * u_mat4Model;
mat4 g_mat4InvTransposeModelView = transpose(inverse(g_mat4ModelView));

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

	mat3 tangentBitangentNormalMatrix = CalculateTBNMatrix(g_mat4InvTransposeModelView, DataIn.tangent, DataIn.normal);
	
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

	vec4 colorSpecular = material.m_colorSpecular; 
	if(u_hasTextureSpecular) {
		colorSpecular *= texture(u_textureSpecular, uvCoord);
	}

	vec4 colorAmbient = material.m_colorAmbient; 
	if (u_hasTextureAmbient) {
		colorAmbient *= texture(u_textureAmbient, uvCoord);
	}

	vec4 colorDiffuse = material.m_colorDiffuse;
	if(u_hasTextureDiffuse) {
		colorDiffuse *= texture(u_textureDiffuse, uvCoord);

		// TODO: this is a bandage
		colorSpecular *= texture(u_textureDiffuse, uvCoord);
		colorAmbient *= texture(u_textureDiffuse, uvCoord);
	}
	

	//vec3 directionEye = normalize(-DataIn.vertTBNSpace);
	vec3 directionEye = tangentBitangentNormalMatrix * normalize(DataIn.directionEye);

	for(int i = 0; i < numLights; i++) {
		//vec3 directionLight = normalize(DataIn.directionLight[i]);
		
		vec3 directionLight = tangentBitangentNormalMatrix * normalize(DataIn.directionLight[i]);
		

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
	
	// Irradiance Mapping (toy BRDF)
	if(u_hasCubemapIrradiance == true) {
		lightColorAmbient = colorDiffuse * ambientIBL(u_cubemapIrradiance, u_mat4Model, u_vec4Eye, DataIn.vertWorldSpace, DataIn.normalOrig, true);
		lightColorAmbient *= 2.0f;
	}

	vec4 outColor = max(vec4LightValue, lightColorAmbient);	

	// Increasing the saturation
	if (u_fAREnabled) {
		out_vec4Color = IncreaseColorSaturation(outColor);
	}

	if(u_hasCubemapEnvironment) {
		if(material.m_reflectivity > 0.0f) {
			vec4 colorReflect = reflectionIBL(u_cubemapEnvironment, u_mat4Model, u_vec4Eye, DataIn.vertWorldSpace, DataIn.normalOrig);
			outColor = mix(outColor, colorReflect, material.m_reflectivity);
		}

		if(material.m_refractivity > 0.0f) {
			vec4 colorRefract = refractionIBL(u_cubemapEnvironment, u_mat4Model, u_vec4Eye, DataIn.vertWorldSpace, DataIn.normalOrig);
			outColor = mix(outColor, colorRefract, material.m_refractivity);
		}
	}

	

	out_vec4Color = outColor;
}
