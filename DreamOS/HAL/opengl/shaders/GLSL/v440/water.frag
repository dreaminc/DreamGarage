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
//uniform sampler2DMS u_textureReflection;

uniform bool	u_hasTextureRefraction;
uniform sampler2D u_textureRefraction;
//uniform sampler2DMS u_textureRefraction;

uniform bool	u_hasTextureNormal;
uniform sampler2D u_textureNormal;
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
uniform vec4 u_vec4Eye;
uniform mat4 u_mat4Projection;
uniform mat4 u_mat4Reflection;

float g_normalDisplacementFactor = 0.025f;

float g_refractiveIndexAir = 1.0f;
float g_refractiveIndexWater = 1.333f;
float g_refractiveIndexGlass = 1.52f;

// TODO: This might need to be done at the refraction program instead
vec3 GetRefractionVector(in vec3 vDirection, in vec3 vNormal, in float refractiveIndexSource, in float refractiveIndexMedium) {
	float refractiveRatio = refractiveIndexSource / refractiveIndexSource;
	float rR2 = pow(refractiveRatio, 2);

	float cVal = dot((-1.0f * vNormal), (vDirection));
	float cVal2 = pow(cVal, 2);

	vec3 vRefraction = refractiveRatio * vDirection;
	vRefraction = vRefraction + (refractiveRatio*cVal - sqrt(1.0f - rR2 * (1.0f - cVal2))) * vNormal;

	return normalize(vRefraction);
}

// This is an approximation of the fresnel equation
float GetFresnelReflectionCoefficient(in vec3 vDirection, in vec3 vNormal) {
	float Rmin = 0.1f;
	float cVal = dot((-1.0f * vNormal), (vDirection));

	float reflectionCoefficient = Rmin + (1.0f - Rmin) * pow((1.0f - cVal), 5);

	reflectionCoefficient = clamp(reflectionCoefficient, Rmin, 1.0f);

	return reflectionCoefficient;
}

// TODO: Move to CPU side
mat4 xzFlipMatrix = mat4(1.0f, 0.0f, 0.0f, 0.0f,
						 0.0f, -1.0f, 0.0f, 0.0f,
						 0.0f, 0.0f, 1.0f, 0.0f,
						 0.0f, 0.0f, 0.0f, 1.0f);

void main(void) {  
	
	vec3 directionEye = normalize(-DataIn.vertTBNSpace);

	vec4 vec4LightValue = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	float diffuseValue = 0.0f;
	float specularValue = 0.0f;
	
	vec3 TBNNormal = vec3(0.0f, 0.0f, 1.0f);
	
	if(u_hasTextureNormal == true) {
		// tiling
		vec2 uvCoord = DataIn.uvCoord * 30.0f;
		uvCoord = mod(uvCoord, 1.0f);
		//uvCoord.x = mod(uvCoord.x, 1.0f);
		//uvCoord.y = mod(uvCoord.y, 1.0f);

		TBNNormal = texture(u_textureNormal, uvCoord).rgb;
		TBNNormal.z *= 2.0f;
		TBNNormal = normalize(TBNNormal * 2.0f - 1.0f); 
	}
	
	// Reflection
	mat4 mat4ReflectedView = xzFlipMatrix * (u_mat4Reflection - mat4(1.0f)) * u_mat4View;

	vec4 vClipReflection = u_mat4Projection * mat4ReflectedView * DataIn.vertWorldSpace;
	vec2 vDeviceReflection = vClipReflection.st / vClipReflection.q;
	vec2 vTextureReflection = vec2(0.5f, 0.5f) + 0.5f * vDeviceReflection;

	float reflectionCoefficient = GetFresnelReflectionCoefficient(-directionEye, TBNNormal);
	//reflectionCoefficient = 1.0f - reflectionCoefficient;
	//reflectionCoefficient = 1.0f;

	float refractionCoefficient = 1.0f - reflectionCoefficient;

	// Refraction

	// We can do refraction entirely in the displacement map
	vec4 vClipRefraction = u_mat4Projection * u_mat4View * DataIn.vertWorldSpace;
	vec2 vDeviceRefraction = vClipRefraction.st / vClipRefraction.q;
	vec2 vTextureRefraction = vec2(0.5f, 0.5f) + 0.5f * vDeviceRefraction;

	// TODO: This is a simplification - the right approach will be to use CPU side on the projection piece
	//vec3 vRefraction = GetRefractionVector(-directionEye, TBNNormal, g_refractiveIndexAir, g_refractiveIndexWater);

	vec4 colorDiffuse = vec4(1.0f); 

	if(u_hasTextureReflection) {
		//colorDiffuse = colorDiffuse * texture(u_textureReflection, DataIn.uvCoord * 1.0f);
		
		// Displace Normals
		vTextureReflection.x += g_normalDisplacementFactor * TBNNormal.x;
		vTextureReflection.y += g_normalDisplacementFactor * TBNNormal.y;

		// TODO: Need to add actual fresnel term and shit

		colorDiffuse = material.m_colorDiffuse * (reflectionCoefficient * texture(u_textureReflection, vTextureReflection));
		
	}

	if(u_hasTextureRefraction) {

		// Displace Normals
		vTextureRefraction.x += g_normalDisplacementFactor * TBNNormal.x;
		vTextureRefraction.y += g_normalDisplacementFactor * TBNNormal.y;

		//vTextureRefraction.x -= (g_normalDisplacementFactor/5) * vRefraction.x;
		//vTextureRefraction.y -= (g_normalDisplacementFactor/5) * vRefraction.y;
		

		// TODO: Need to add actual refractive index and shit

		colorDiffuse = colorDiffuse + (refractionCoefficient * texture(u_textureRefraction, vTextureRefraction));
	}

	vec4 colorAmbient = material.m_ambient * material.m_colorAmbient;

	

	for(int i = 0; i < numLights; i++) {
		vec3 directionLight = normalize(DataIn.directionLight[i]);

		if(dot(TBNNormal, directionLight) > 0.0f) {
			CalculateFragmentLightValue(lights[i].m_power, TBNNormal, directionLight, directionEye, DataIn.distanceLight[i], diffuseValue, specularValue);
			
			vec4LightValue += diffuseValue * lights[i].m_colorDiffuse * colorDiffuse;
			vec4LightValue += specularValue * lights[i].m_colorSpecular * material.m_colorSpecular;
		}
	}

	out_vec4Color = vec4(max(vec4LightValue.xyz, colorAmbient.xyz), colorDiffuse.a);// + vec4(0.1f, 0.1f, 0.1f, 1.0f);
}
