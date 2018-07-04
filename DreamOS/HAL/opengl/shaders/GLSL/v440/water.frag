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

uniform bool	u_hasTextureRefractionDepth;
uniform sampler2D u_textureRefractionDepth;
//uniform sampler2DMS u_textureRefractionDepth;

uniform bool	u_hasTextureNormal;
uniform sampler2D u_textureNormal;

uniform float u_time;

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

	float depthOfPoint = (u_mat4View * DataIn.vertWorldSpace).z;
	
	if(u_hasTextureNormal == true) {
		// tiling
		vec2 uvCoord = DataIn.uvCoord * 5.0f;
		uvCoord = mod(uvCoord, 1.0f);

		TBNNormal = texture(u_textureNormal, uvCoord).rgb;
		TBNNormal.z *= 2.0f;
		TBNNormal = normalize(TBNNormal * 2.0f - 1.0f); 
	}
	else {
		vec2 pos = vec2(DataIn.uvCoord * 500.0);
		
		vec3 normalHHHF = getNoiseNormal(vec2(DataIn.uvCoord * 500000.0) + 0.5f * u_time);
		vec3 normalHHF = getNoiseNormal(vec2(DataIn.uvCoord * 5000.0) - 0.25f * u_time);
		vec3 normalHF = getNoiseNormal(vec2(DataIn.uvCoord * 1500.0) + 0.3f * u_time);
		vec3 normalLF = getNoiseNormal(vec2(DataIn.uvCoord * 100.0) - 0.1f * u_time);
		
		
		TBNNormal = 0.45f * normalHHHF + 0.65f * normalHHF + 1.0f * normalHF + 0.25f * normalLF;
		TBNNormal = normalize(TBNNormal);
		
		TBNNormal = mix(TBNNormal, vec3(0.0f, 0.0f, 1.0f), clamp(abs(depthOfPoint)/150.0f, 0.0f, 1.0f));
		TBNNormal = normalize(TBNNormal);
	}
	
	// Reflection
	mat4 mat4ReflectedView = xzFlipMatrix * (u_mat4Reflection - mat4(1.0f)) * u_mat4View;

	vec4 vClipReflection = u_mat4Projection * mat4ReflectedView * DataIn.vertWorldSpace;
	vec2 vDeviceReflection = vClipReflection.st / vClipReflection.q;
	vec2 vTextureReflection = vec2(0.5f, 0.5f) + 0.5f * vDeviceReflection;

	float reflectionCoefficient = GetFresnelReflectionCoefficient(-directionEye, TBNNormal);
	//reflectionCoefficient = 1.0f;

	float refractionCoefficient = 1.0f - reflectionCoefficient;

	// Refraction

	// We can do refraction entirely in the displacement map
	vec4 vClipRefraction = u_mat4Projection * u_mat4View * DataIn.vertWorldSpace;
	vec2 vDeviceRefraction = vClipRefraction.st / vClipRefraction.q;
	vec2 vTextureRefraction = vec2(0.5f, 0.5f) + 0.5f * vDeviceRefraction;

	// Displace Normals for refraction 
	vTextureRefraction.x += g_normalDisplacementFactor * TBNNormal.x;
	vTextureRefraction.y += g_normalDisplacementFactor * TBNNormal.y;

	// Displace Normals for reflection
	vTextureReflection.x += g_normalDisplacementFactor * TBNNormal.x;
	vTextureReflection.y += g_normalDisplacementFactor * TBNNormal.y;

	vec4 colorDiffuse = material.m_colorDiffuse; 

	float waterOpacity = 0.0f;

	if(u_hasTextureReflection) {
		//colorDiffuse = colorDiffuse * texture(u_textureReflection, DataIn.uvCoord * 1.0f);

		// TODO: Need to add actual fresnel term and shit

		// Blend with reflection 
		vec4 colorReflection = texture(u_textureReflection, vTextureReflection);
		colorDiffuse = mix(colorDiffuse, colorReflection, reflectionCoefficient);
		//colorDiffuse = mix(colorDiffuse, colorReflection, min((1.0f - waterOpacity + 0.1f), reflectionCoefficient));
	}

	if(u_hasTextureRefraction) {	
		// TODO: Need to add actual refractive index and shit
	
		vec4 colorRefraction = refractionCoefficient * (texture(u_textureRefraction, vTextureRefraction));
		//colorDiffuse = mix(colorDiffuse, colorRefraction, refractionCoefficient);
		//colorDiffuse = mix(colorDiffuse, colorRefraction, 1.0f - waterOpacity);
	
		float zRefractionDepth = u_mat4Projection[3].z / (texture(u_textureRefractionDepth, vTextureRefraction).x * -2.0 + 1.0 - u_mat4Projection[2].z);
		//float depthOfPoint = (u_mat4View * DataIn.vertWorldSpace).z;
		float waterDepth = depthOfPoint - zRefractionDepth;
	
		// Water Color / Opacity
		// Ctint = Cwater * (Omin + (1 - Omin) * sqrt (min (thickness / Dopaque, 1)))
	
		float minWaterOpacity = 0.1f;
		float depthOpaque = 1.0f;
		
		waterOpacity = (minWaterOpacity + (1.0f - minWaterOpacity) * (min(waterDepth / depthOpaque, 1.0f)));
		
		colorRefraction = mix(vec4(0.0f, 0.0f, 0.15f, 1.0f), colorRefraction, 1.0f - waterOpacity);
		colorDiffuse = mix(colorDiffuse, colorRefraction, refractionCoefficient);
	}

	

	vec4 colorAmbient = material.m_ambient * material.m_colorAmbient;

	for(int i = 0; i < numLights; i++) {
		vec3 directionLight = normalize(DataIn.directionLight[i]);
	
		if(dot(TBNNormal, directionLight) > 0.0f) {
			// provide shine param
			CalculateFragmentLightValue(lights[i].m_power, 50.0f, TBNNormal, directionLight, directionEye, DataIn.distanceLight[i], diffuseValue, specularValue);
			
			vec4LightValue += diffuseValue * lights[i].m_colorDiffuse * colorDiffuse;
			//vec4LightValue += specularValue * lights[i].m_colorSpecular * material.m_colorSpecular;
			vec4LightValue += specularValue * lights[i].m_colorSpecular * vec4(1.0f);
		}
	}

	out_vec4Color = vec4LightValue; //, colorAmbient.xyz), colorDiffuse.a);// + vec4(0.1f, 0.1f, 0.1f, 1.0f);


	//out_vec4Color = colorDiffuse;
}
