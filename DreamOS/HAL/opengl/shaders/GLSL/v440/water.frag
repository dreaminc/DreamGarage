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
	vec4 reflectedVert;
	vec4 refractedVert;
	float vertDepth; 
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
uniform mat4 u_mat4ViewProjection;
uniform mat4 u_mat4Projection;
uniform mat4 u_mat4Reflection;

// Was 0.025 before
float g_normalDisplacementFactor = 0.10f;

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

///*
vec3 noiseNormal(vec2 uvCoord) {
	vec3 normalHHHF = getNoiseNormal(vec2(uvCoord * 500000.0) + 0.5f * u_time);
	vec3 normalHHF = getNoiseNormal(vec2(uvCoord * 5000.0) - 0.25f * u_time);
	vec3 normalHF = getNoiseNormal(vec2(uvCoord * 1500.0) + 0.3f * u_time);
	vec3 normalLF = getNoiseNormal(vec2(uvCoord * 100.0) - 0.1f * u_time);
		
	vec3 vNormal = 0.0f * normalHHHF + 0.05f * normalHHF + 1.0f * normalHF + 0.25f * normalLF;
	//vec3 vNormal = 1.0f * normalHHF + 0.5f * normalHF;
	vNormal = normalize(vNormal);
		
	vNormal = mix(vNormal, vec3(0.0f, 0.0f, 1.0f), clamp(abs(DataIn.vertDepth) / 100.0f, 0.0f, 1.0f));
	vNormal.z *= 5.0f;
	vNormal = normalize(vNormal);

	return vNormal;
}

float noiseHeight(vec2 uvCoord) {
	float valHHHF = noise(vec2(uvCoord * 500000.0) + 0.5f * u_time);
	float valHHF = noise(vec2(uvCoord * 5000.0) - 0.25f * u_time);
	float valHF = noise(vec2(uvCoord * 1500.0) + 0.3f * u_time);
	float valLF = noise(vec2(uvCoord * 100.0) - 0.1f * u_time);
		
	float val = 0.0f * valHHHF + 0.05f * valHHF + 1.0f * valHF + 0.25f * valLF;
	//float val = 1.0f * valHHF + 0.5f * valHF;
	
	val = mix(val, 0.0f, clamp(abs(DataIn.vertDepth) / 100.0f, 0.0f, 1.0f));
	val /= 5.0f;

	val = val * 0.5f + 0.5f;
	
	return val;
}
//*/

float g_scaling = 1000.0f;

/*
// simple
vec3 noiseNormal(vec2 uvCoord) {
	vec3 vNormal = getNoiseNormal(vec2(uvCoord * g_scaling) - 0.1f * u_time);
	return normalize(vNormal);
}

float noiseHeight(vec2 uvCoord) {
	float val = noise(vec2(uvCoord * g_scaling) - 0.1f * u_time) * 0.5f + 0.5f;
	return val;
}
//*/

// Define this function (used to look up)
float ParallaxMapping_HeightFunction(vec2 uvCoord) {
	// TODO: displacement texture?

	return noiseHeight(uvCoord);
}

void main(void) {  
	
	vec3 directionEye = normalize(-DataIn.vertTBNSpace);

	// un comment this to apply parallax to water
	//vec2 texCoords = ParallaxMapping(DataIn.uvCoord, directionEye, material.m_displacement);
	vec2 texCoords = ParallaxMapping(DataIn.uvCoord, directionEye, 0.1f/15);
	//vec2 texCoords = DataIn.uvCoord;

	vec4 vec4LightValue = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	float diffuseValue = 0.0f;
	float specularValue = 0.0f;
	
	vec3 TBNNormal = vec3(0.0f, 0.0f, 1.0f);
	
	if(u_hasTextureNormal == true) {
		// tiling
		vec2 uvCoord = DataIn.uvCoord * 5.0f;
		uvCoord = mod(uvCoord, 1.0f);

		TBNNormal = texture(u_textureNormal, uvCoord).rgb;
		TBNNormal.z *= 2.0f;
		TBNNormal = normalize(TBNNormal * 2.0f - 1.0f); 
	}
	else {
		TBNNormal = noiseNormal(texCoords);

		// Turn off bumps
		//TBNNormal = vec3(0.0f, 0.0f, 1.0f);
	}
	
	// Reflection
	vec2 vDeviceReflection = DataIn.reflectedVert.st / DataIn.reflectedVert.q;
	vec2 vTextureReflection = vec2(0.5f, 0.5f) + 0.5f * vDeviceReflection;

	float reflectionCoefficient = GetFresnelReflectionCoefficient(-directionEye, TBNNormal);
	float refractionCoefficient = 1.0f - reflectionCoefficient;

	// Refraction

	// We can do refraction entirely in the displacement map
	vec2 vDeviceRefraction = DataIn.refractedVert.st / DataIn.refractedVert.q;
	vec2 vTextureRefraction = vec2(0.5f, 0.5f) + 0.5f * vDeviceRefraction;

	// Displace Normals for refraction and reflection 
	vTextureRefraction += g_normalDisplacementFactor * TBNNormal.xy;
	vTextureReflection += g_normalDisplacementFactor * TBNNormal.xy;

	vec4 colorDiffuse = material.m_colorDiffuse; 
	//vec4 colorDiffuse = vec4(57.0f / 255.0f, 112.0f / 255.0f, 151.0f / 255.0f, 1.0f); 

	float waterOpacity = 0.0f;

	if(u_hasTextureReflection) {
		// Blend with reflection 
		vec4 colorReflection = texture(u_textureReflection, vTextureReflection);
		colorDiffuse = mix(colorDiffuse, colorReflection, reflectionCoefficient);
	}

	if(u_hasTextureRefraction) {	
		vec4 colorRefraction = refractionCoefficient * (texture(u_textureRefraction, vTextureRefraction));
	
		float zRefractionDepth = u_mat4Projection[3].z / (texture(u_textureRefractionDepth, vTextureRefraction).x * -2.0f + 1.0f - u_mat4Projection[2].z);
		float waterDepth = DataIn.vertDepth - zRefractionDepth;
	
		// Water Color / Opacity
		// Ctint = Cwater * (Omin + (1 - Omin) * sqrt (min (thickness / Dopaque, 1)))
	
		float minWaterOpacity = 0.025f;
		float depthOpaque = 4.0f;
		
		waterOpacity = (minWaterOpacity + (1.0f - minWaterOpacity) * (min(waterDepth / depthOpaque, 1.0f)));
		
		//colorRefraction = mix(vec4(0.0f, 0.0f, 0.15f, 1.0f), colorRefraction, 1.0f - waterOpacity);
		colorRefraction = mix(colorDiffuse, colorRefraction, 1.0f - waterOpacity);
		colorDiffuse = mix(colorDiffuse, colorRefraction, refractionCoefficient);
	}

	vec4 colorAmbient = material.m_ambient * material.m_colorAmbient;

	// This looks a lot better
	vec4LightValue = colorDiffuse;

	for(int i = 0; i < numLights; i++) {
		vec3 directionLight = normalize(DataIn.directionLight[i]);
	
		if(dot(TBNNormal, directionLight) > 0.0f) {
			CalculateFragmentLightValue(lights[i].m_power, material.m_shine, TBNNormal, directionLight, directionEye, DataIn.distanceLight[i], diffuseValue, specularValue);
			
			//vec4LightValue += diffuseValue * lights[i].m_colorDiffuse * colorDiffuse;
			vec4LightValue += specularValue * lights[i].m_colorSpecular * material.m_colorSpecular;
			//vec4LightValue += specularValue * lights[i].m_colorSpecular * vec4(1.0f);
		}
	}

	out_vec4Color = vec4LightValue; //, colorAmbient.xyz), colorDiffuse.a);// + vec4(0.1f, 0.1f, 0.1f, 1.0f);
}
